/* Copyright (c) 2006-2011, Universities Space Research Association (USRA).
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

var GRID_SETTINGS = {
	"pixelsPerTimeIncrement": 20,
	"tokenHeight": 15,			// 15px
	"tokenVerticalGap":4,		// vertical separation between tokens in px TODO doesn't scale yet...
	// computed
	"maxTimeIncrements": 30,	// default, see computeMaxTimeIncrements
	
	// TODO This should now really be called GANTT_SETTINGS and moved to main.js
	"objectColumnWidth": 150,	// hardcoded for now until dynamically computed based on names widths 
	"columnBuffer": 6, 			// 8 gives some space between columns
	"gridLabelHeight": 15
}

function drawGrid() {
	computeMaxTimeIncrements();
// was	// $(".gHS").each(function(idx, el) {drawGridSection(el)});
	$("div.tokenColumnRow").each(function(idx, el) {drawGridSection(el)});	
	drawGridHeader(); // not yet done...
	// now we know how big to make the token column width
	sizeTokenColumn();	
	sizeObjectColumn();  // TODO move this into sizing? and call from elsewhere
	sizeGantt();
}

/* 
 * Computes maxTimeIncrements on GRID_SETTINGS
 */
function computeMaxTimeIncrements() {
	var allEnds;
	var maxEnd; // max End Time Increment from all tokens
	var allTokenDivs = $("#gantt .tokenLine");

	// compute grid width based on max end time of tokens
	allEnds = $(allTokenDivs).map(function (idx, el) {return parseInt($(el).data("token").end, 10)});
	maxEnd = allEnds.toArray().reduce( // reduce is EMCA 5
		function (max, el) {return (max > el) ? max : el}, 0);
	// console.log("All ends: ", allEnds);
	// console.log("Max end: ", maxEnd);
	
	// for extra buffer, add 10 and round up to nearest 10
	maxEnd = maxEnd - (maxEnd % 10) + 10;
	// if less than 30, default to 30...
	if (maxEnd < 30) {maxEnd = 30};
	// store for use later
	GRID_SETTINGS.maxTimeIncrements = maxEnd;
}

function drawGridSection(parentDiv) {
	var gridSection = $("<div class='gridSection'></div>").prependTo(parentDiv);
	drawGridLines(gridSection);
}

/*
 * Create gridLines div and add to gridSection
 */
function drawGridLines(gridSection) {
	// configuration
	var numGridLines = GRID_SETTINGS.maxTimeIncrements + 1; // +1 since we start drawing at zero
	console.log(numGridLines);
	//8/2/11 Isaac
	//numGridLines = GRID_SETTINGS.maxTimeIncrements + 1;
	//numGridLines = plexilscaling * GRID_SETTINGS.maxTimeIncrements + 1;
	var width = getPixelsPerTimeIncrement();
	//var width			= GRID_SETTINGS.pixelsPerTimeIncrement;
	// internal
	var gridLines; // new div will be created
	var line;			// new lines will be created too

	// set height
	$(gridSection).css("height", $(gridSection).parent().css("height"));

	gridLines = $("<div class='gridLines'></div>").appendTo(gridSection);

    // also create major vertical grid line divs
	for (var i = 0; i < numGridLines; i += 10) {
		line = $("<div class='gridLine'></div>");
		line.css("left", width * i);
		$(gridLines).append(line);	
		if (i > 0) {
		 	line.addClass("majorGridLine");
		}
	}

	// line = $("<div class='gridLine'></div>");
	// 	line.css("left", width *= 10);
	// 	$(gridLines).append(line);	
	// 	//if (i > 0) {
	// 	 	line.addClass("majorGridLine");
	// //	}

	// old slower implementation below
	// // also create individual vertical grid line divs
	// for (var i = 0; i < numGridLines; i++) {
	// 	line = $("<div class='gridLine'></div>");
	// 	line.css("left", width * i);
	// 	$(gridLines).append(line);	
	// 	if ((i > 0) && ((i % 10) === 0)) {
	// 		line.addClass("majorGridLine");
	// 	} else {
	// 		line.addClass("minorGridLine");
	// 	} 
	// }
}

function drawGridHeader() {
	var myscaling = getScaling();
	var header = $("<div class='gridHeader'></div>").prependTo($('.tokenColumn'));
	//var header = $("<div class='gridHeader'></div>").prependTo($("#gantt"));
	$('.gridHeader').css('left','0px');
	var majorGridLines;

	drawGridLines(header);

	// override height with a static value
	$(header).css("height", GRID_SETTINGS.gridLabelHeight); 
	
	majorGridLines = $("div.gridHeader div.majorGridLine");
	for (var i=0; i<= (majorGridLines.length - 2); i++) {
		//aoccomodates plexilscaling and selected scaling; plexilscaling displays tokens as their actual time
		$(majorGridLines[i]).html("<span class='majorGridLabel'>" + (10+(i*10))/(plexilscaling/myscaling) + "</span>");
	}
}

function handleSliderChange(e, ui)
{
  var maxScroll = $(".tokenColumn").attr("scrollWidth") - $("#content-scroll").width();
  $(".tokenColumn").animate({scrollLeft: ui.value * (maxScroll / 100) }, 1000);
}

function handleSliderSlide(e, ui)
{
  var maxScroll = $(".tokenColumn").attr("scrollWidth") - $("#content-scroll").width();
  $(".tokenColumn").attr({scrollLeft: ui.value * (maxScroll / 100) });
}