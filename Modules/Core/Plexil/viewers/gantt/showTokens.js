/* Copyright (c) 2006-2012, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Modified by Madan, Isaac A.
* Originally authored by Swanson, Keith J. (ARC-TI)
*/

//needed to accomodate small times
var plexilscaling = 100;

function showTokens(allTokens) {
    var sortedTokens = sortTokens(allTokens);
    layoutStaticStructure();	
    groupAndShowTokens(sortedTokens);
    attachHoverStyling();
    createDialogBoxes(allTokens);
    keepTokens(allTokens);
}

function sortTokens(allTokens) {
    return allTokens.sort(tokenSortFunction); // alphabetic sort	
}

function layoutStaticStructure() {
    $("#gantt").append([
	'<div class="ganttColumns" >',
	'<div class="objectColumn" />',
	'<div class="tokenColumn" />',
	'</div>'
    ].join(''));	
}

function groupAndShowTokens(tokens) {
    var gantt = $("#gantt");
    var tokenHeight = getTokenHeight();
    var tokenVerticalGap = GRID_SETTINGS.tokenVerticalGap;
    var objectColumn	= $("div.objectColumn");
    var objectColumnRow;
    var tokenColumn		= $("div.tokenColumn");
    var tokenColumnRow;
    var tokenColumnRowTokens;
    var objectNames		= getSortedObjectNames(tokens);	
    var tokensOfObject 	= [];
    var firstPredicateToken;
    var tokenRowsCount;
    var tokensHeight;
    var objectTitleHeight;
    var sectionHeight;
    
    for (var i=0; i<objectNames.length; i++) {
	objectColumnRow	= $("<div class='objectColumnRow'></div>").appendTo(objectColumn);
	//infoColumnRow = $("<div class='infoColumnRow'>hello</div>").appendTo(infoColumn);
	tokenColumnRow	= $("<div class='tokenColumnRow'></div>").appendTo(tokenColumn);
	tokensOfObject = getTokensOfObject(tokens, objectNames[i]);
	tokenRowsCount = 0;
	
	// set alternating background color
	if ((i % 2) === 0) objectColumnRow.addClass('gHSAlternateBG');
	if ((i % 2) === 0) tokenColumnRow.addClass('gHSAlternateBG');

	// fill in LHS, object name first		
	$(objectColumnRow).append("<div class='objectTitle'>" + objectNames[i] + "</div>");
	

	// finish LHS, now showing predicate names
	// Assume tokens are sorted with all OPplan tokens together, followed by all OPsim tokens together.
	for (var j=0; j<tokensOfObject.length; j++){
	    firstPredicateToken = (j===0) || !sameOP(tokensOfObject[j], tokensOfObject[j-1]);
	    if (firstPredicateToken) {
		if (futureOPsim(tokensOfObject.slice(j))) {
		    //Isaac -- match line 75
		    //$(objectColumnRow).append("<div class='objectInfo'>" + tokensOfObject[j].id + "</div>");
		    
		    // plan and sim tokens for this predicate
		    $(objectColumnRow).append("<div class='predicateTitle predicateTitleDoubleHeight'>" +
					      tokensOfObject[j].predicateName + "</div>");	
		    tokenRowsCount = tokenRowsCount + 2;
		} else {
		    //Isaac -- match line 66
		    //$(objectColumnRow).append("<div class='objectInfo'>" + tokensOfObject[j].id + "</div>");
		    
		    // only plan or sim tokens for this predicate
		    //Isaac 7/21/11
		    if(showExpanded == "true") {
			$(objectColumnRow).append("<div class='predicateTitle'>" +
						  tokensOfObject[j].predicateName + "</div>");
			tokenRowsCount++;
		    }
		    else {
			$(objectColumnRow).append("<div class='predicateTitle'></div>");
			$('.objectInfo').hide();
			tokenRowsCount = 1;
		    }
		}
	    }
	}

	// fill in RHS
	$(tokenColumnRow).append("<div class='objectTitle' />"); // on RHS, this is just a spacer
	tokenColumnRowTokens = $("<div class='tokenColumnRowTokens' />").appendTo(tokenColumnRow);
	showTokenSet(tokensOfObject, tokenColumnRowTokens);
	
	// now figure out height of rows based on tokens
	tokenRowHeight	= (tokenHeight + tokenVerticalGap);
	tokensHeight =  tokenRowsCount * tokenRowHeight;
	// object title height
	objectTitleHeight = parseInt($(".objectTitle").css("height"), 10);
	// now set various heights
	$(".predicateTitle").css("height", tokenRowHeight);
	$(".predicateTitleDoubleHeight").css("height", (2 * tokenRowHeight));
	sectionHeight = tokensHeight + objectTitleHeight;
	objectColumnRow.css("height", sectionHeight);
	tokenColumnRow.css("height", sectionHeight);
	tokenColumnRowTokens.css("height", tokensHeight);

	// force the predicateNames to be vertically aligned
	$(".predicateTitle").css("line-height", tokenRowHeight + "px");	// pure number multiplies by font size
	$(".predicateTitleDoubleHeight").css("line-height", (2 * tokenRowHeight) + "px");
    }
}

function showTokenSet(tokens, parentDiv) {
    var myscaling = getScaling();
    
    // context
    var timeStepIncrement = getPixelsPerTimeIncrement();
    //var timeStepIncrement	= GRID_SETTINGS.pixelsPerTimeIncrement;
    var tokenHeight = getTokenHeight();
    //var tokenHeight			= GRID_SETTINGS.tokenHeight;
    var tokenVerticalGap	= GRID_SETTINGS.tokenVerticalGap;
    // internal
    var tokenLine;			// an individual token div
    var leftOffset;			// how far to left edge of the tokenLine
    var topOffset;			// how far down in this section to draw
    var topOffsetCount = 0;	// how many rows of tokens so far
    var lineWidth;			// width of each individual tokenLine (duration)
    var tokenBorderOffset;	// used to factor in token border width in layout
    
    var parentCondition;
    
    for (var i=0; i<tokens.length; i++) {

	// create plan and sim divs
	tokenLine	= $("<div class='tokenLine'></div>").appendTo(parentDiv);
	if (tokens[i].role === "plan") {
	    $(tokenLine).addClass("planLine");
	} else if (tokens[i].role === "sim") {
	    $(tokenLine).addClass("simLine");
	}
	
	if(tokens[i].predicateName == tokens[i].objectName) $(tokenLine).addClass("topLine");
	
	parentCondition = false;
	for(var j = 0; j < tokens.length; j++) {
	    if(tokens[j].predicateName == tokens[i].objectName) parentCondition = true;
	}
	if(parentCondition && tokens[i].predicateName != tokens[i].objectName) $(tokenLine).addClass("parentLine");
	
	// store a pointer to the token object
	$(tokenLine).data("token", tokens[i]);

	// get border width and assume all 4 borders are the same 
	tokenBorderOffset = getTokenBorderWidth(tokenLine);

	// compute line width

        // KMD: These were previously commented out.  Retaining in case they capture some useful (lost) insight.
        // lineWidth = timeStepIncrement * (tokens[i].end - tokens[i].start);
        // lineWidth = timeStepIncrement * (tokens[i].endDomain*plexilscaling - tokens[i].startDomain*plexilscaling);

        // KMD: The applicaiton of abs works around an incorrect width
        // seen in display when the line width is otherwise negative.
        // Not sure if this entirely fixes the problem, or if there is a
        // better approach.
        lineWidth = Math.abs(((timeStepIncrement * (tokens[i].endDomain - tokens[i].startDomain)) -
                              (tokenBorderOffset * 2))
                             / myscaling);	

	$(tokenLine).css("width", lineWidth);

	// compute left offset
//	leftOffset = timeStepIncrement * tokens[i].start;
	leftOffset = timeStepIncrement * tokens[i].startDomain;
	//leftOffset = timeStepIncrement * tokens[i].start *plexilscaling;
	leftOffset = leftOffset / myscaling;
	leftOffset++; // move over one pixel so tokens end on grid lines
	$(tokenLine).css("left", leftOffset);

	// Compute topOffsetCount so we can
	// put consecutive tokens on the same row if their object name,
	// predicate name, and type (plan/sim) match.
	if ((i > 0) && (!sameOPR(tokens[i], tokens[i-1]))) {
	    topOffsetCount++; // token[i] needs a new row
	}

	// compute top offset for this section
	topOffset = (tokenHeight + tokenVerticalGap) * topOffsetCount ;
	$(tokenLine).css("top", topOffset);
	
	// set heights
	$(tokenLine).css("height", (tokenHeight - (tokenBorderOffset * 2)));

	// compute hover string
	$(tokenLine).attr("title", String(tokens[i].predicateName) +" "+ String(tokens[i].domainParameters[0].value) + " (" + String(convertInfinities(tokens[i].durationDomain)/plexilscaling) + ")");
	
	// If there's a single domain parameter, show its value in tokenLine and
	// force the values to be vertically aligned within the token.
	//Isaac 7/20/11
	if(convertInfinities(tokens[i].durationDomain) != 0)
	{
	    //if (tokens[i].domainParameters.length === 1) { // a single domain parameter
	    $(tokenLine).html("&nbsp;" + String(tokens[i].predicateName) +" "+ 
			      //String(tokens[i].domainParameters[0].value) + 
			      " (" + String(convertInfinities(tokens[i].durationDomain)/plexilscaling) + ")");
	    // $(tokenLine).text("12fadfda5");
	    $(".tokenLine").css("line-height", tokenHeight + "px");	// pure number multiplies by font size				
	    //}
	}
    }
}

/** used to figure out how to group and show tokens **/

// same Object, Predicate, Role(plan/sim)
function sameOPR (tokenA, tokenB) {
    if(showExpanded == "true") {
	return ((tokenA.objectName		=== tokenB.objectName) &&
		(tokenA.predicateName	=== tokenB.predicateName) &&
		(tokenA.role			=== tokenB.role)); 	
    }
    else {
	return ((tokenA.objectName		=== tokenB.objectName) &&
		(tokenA.role			=== tokenB.role)); 	
    }
}

// same Object, Predicate
function sameOP (tokenA, tokenB) {
    return ((tokenA.objectName		=== tokenB.objectName) &&
	    (tokenA.predicateName	=== tokenB.predicateName));
}

// checks if first token is a plan token, and future sameOP is a sim token.
function futureOPsim (tokens) {
    var result = false;
    if (tokens[0].role === "plan") {
	// look for a future sim token
	for (var i=1; i<tokens.length; i++) {
	    if (sameOP(tokens[i], tokens[i-1]) &&
		(tokens[i].role === "sim")) {
		result = true;
		break;
	    }
	}		
    }
    return result;
}

function getTokensOfObject (tokens, objectName) {
    var tokensOfObject = [];
    for (var j=0; j<tokens.length; j++) {
	if (tokens[j].objectName === objectName) {
	    tokensOfObject.push(tokens[j]);
	}
    }
    return tokensOfObject;
}

function getSortedObjectNames (tokens) {
    var objectNodes = [];
    tokens.forEach(
	function(token) {
	    if(objectNodes.indexOf(token) === -1) {
		objectNodes.push(token);
	    }
	});
    objectNodes.sort(nodeSortingFunction);
    var objectNames = [];
    tokens.forEach(
	function(token) {
	    if(objectNames.indexOf(token.objectName) === -1) {
		objectNames.push(token.objectName);
	    }
	});
    return objectNames;
}

function nodeSortingFunction(a, b) {
    var aId = parseFloat(a.id);
    var bId = parseFloat(b.id);
    return aId - bId;
}

// Sort tokens based on domain requirements; root is at the top; 
// children are organized by time, else alphabetically

function tokenSortFunction(a, b) {
    if(a.objectName == a.predicateName && b.objectName != b.predicateName) 
    {
	return -1;
    }
    else if(a.objectName != a.predicateName && b.objectName == b.predicateName) 
    {
	return 1;
    } 
    else 
    {
	if(a.id < b.id) 
	{
	    return -1;
	}
	else if (a.id > b.id) 
	{
	    return 1;
	} 
	else 
	{
	    if(a.start < b.start) 
	    {
		return -1;
	    }
	    else if(a.start > b.start) 
	    {
		return 1;
	    }
	    else 
	    {
		return 0;
	    }
	}
    }
}

/** adds stylistic properties on events **/

function attachHoverStyling() {
    $(".tokenLine").hover(addHoverStyling, removeHoverStyling);
}

function addHoverStyling(event) {
    var tokenLine = event.target; // or could use this
    if (! $(tokenLine).hasClass("selectedTokenLine")) {
	addTokenBorderStyling (tokenLine, "hoverTokenLine");
    }
}

function removeHoverStyling(event) {
    var tokenLine = event.target; // or could use this
    if (! $(tokenLine).hasClass("selectedTokenLine")) {
	removeTokenBorderStyling (tokenLine, "hoverTokenLine");
    }
}

function addSelectionStyling(tokenLine) {
    var existingSelection = $("div.stickyHiliteTokenLine")[0]; 
    removeTokenBorderStyling(existingSelection, "stickyHiliteTokenLine");
    addTokenBorderStyling(tokenLine, "stickyHiliteTokenLine");
}

function addTokenBorderStyling (tokenLine, styleClass) {
    var top		= parseInt($(tokenLine).css("top"), 10);
    var left	= parseInt($(tokenLine).css("left"), 10);			
    var orig	= getTokenBorderWidth(tokenLine);
    $(tokenLine).addClass(styleClass);
    var revised	= getTokenBorderWidth(tokenLine);		
    $(tokenLine).css("top",  top  - (revised - orig));
    $(tokenLine).css("left", left - (revised - orig));	
}

function removeTokenBorderStyling (tokenLine, styleClass) {
    var top 	= parseInt($(tokenLine).css("top"), 10);
    var left	= parseInt($(tokenLine).css("left"), 10);			
    var revised	= getTokenBorderWidth(tokenLine);
    $(tokenLine).removeClass(styleClass);
    var orig	= getTokenBorderWidth(tokenLine);
    $(tokenLine).css("top",  top  + (revised - orig));
    $(tokenLine).css("left", left + (revised - orig));	
}

function getTokenBorderWidth(tokenLine) {
    return parseInt($(tokenLine).css("border-top-width"), 10);
}



