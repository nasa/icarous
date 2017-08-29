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
* By Madan, Isaac A.
* Originally authored by Swanson, Keith J. (ARC-TI)
*/

//boldens selected token
function showTokenDetails(event) {
	var token = $(event.target).data("token");
	addSelectionStyling(event.target);	
}

//add the details to the token box
function tokenDetailsHtml(token) {
	var result = "";
	result += getLocalValuesHtml(token);
	result += "<span class='detailsLabel'>Class: </span>";
	result += "<span class='detailsValue'>" + token.classNames[0] + "</span><br>";
	result += "<span class='detailsLabel'>Instance: </span>";
	result += "<span class='detailsValue'>" + token.objectName + "</span><br>";			
	result += "<span class='detailsLabel'>Predicate: </span>";
	result += "<span class='detailsValue'>" + token.predicateName + "</span><br>";
	result += "<span class='detailsLabel'>Instance: </span>";
	result += "<span class='detailsValue'>" + token.predicateInstanceName + "</span><br><br>";
	result += "<span class='detailsLabel'>Start: </span>";
	result += "<span class='detailsValue'>" + convertInfinities(token.startDomain) + "</span><br>";	
	result += "<span class='detailsLabel'>Duration: </span>";
	result += "<span class='detailsValue'>" + convertInfinities(token.durationDomain) + "</span><br>";
	result += "<span class='detailsLabel'>End: </span>";
	result += "<span class='detailsValue'>" + convertInfinities(token.endDomain) + "</span><br>";	

	return result; 	
}

//add additional details to the token box (custom items such as children, local variables)
function getLocalValuesHtml(token) {
	var result = "";
	var params = token.domainParameters;
	for (var k = 0; k < params.length; k++) {
		result += "<span class='detailsLabel'>" + params[k].name + ": </span>";
		result += "<span class='detailsValue'>" + convertInfinities(params[k].value) + "</span><br>";
	}
	return result + "<br>";
}

/** Grab details from tokens and store them in the details box
*	predicateName is node name
*	classNames[0] is parent name.node name
*	objectName is parent name
*	predicateInstanceName is node type (ie Command)
**/
function createDialogBoxes(tokens) {
	setupFooter(tokens.length);
	loopTracker(tokens);
	var i = 0;
	for(i = 0; i < tokens.length; i++) {
		var loopCount;
		if(myLoops[tokens[i].id] > 1) loopCount = myLoops[tokens[i].id];
		else loopCount = "No loop";
		$('#gantt').append([
						  '<div id="dialogBox',
						  i,
						  '" title="',
						  String(tokens[i].classNames[0]),
						  '">',
						  //'<strong>This Node: </strong>',
						  //tokens[i].predicateName,
						  //'<br>',
						  '<strong>Node Type: </strong>',
						  tokens[i].predicateInstanceName,
						  '<br>',
						  //'<strong>Parent Node: </strong>',
						  //tokens[i].objectName,
						  //'<br>',
						  '<strong>Start Time: </strong>',
						  convertInfinities(tokens[i].startDomain)/plexilscaling,
						  '<br>',
						  '<strong>End Time: </strong>',
						  convertInfinities(tokens[i].endDomain)/plexilscaling,
						  '<br>',
						  '<strong>Duration: </strong>',
						  convertInfinities(tokens[i].durationDomain)/plexilscaling,
						  '<br>',
						  //'<strong>Node State: </strong>',
						  //String(tokens[i].domainParameters[0].value),
						  //'<br>',
						  '<strong>Execution order: </strong>',
						  tokens[i].id,
						  '<br>',
						  '<strong>Loop count: </strong>',
						  loopCount,
						  '<br>',
						  '<strong>Child Nodes: </strong>',
						  String(tokens[i].domainParameters[1].value),
						  '<br>',
						  '<strong>Local Variables (name = val. before --> val. after): </strong>',
						  String(tokens[i].domainParameters[2].value),
						  '</div>'
						  ].join(''));
		$('#dialogBox'+i).dialog({
								 autoOpen:false,
								 height:250,
								 width:400
								 });
		$('#dialogBox'+i).dialog("close");
		$('#dialogBox'+i).dialog( { stack: true } );
	}
	myLoops.splice(0, myLoops.length);
}

//footer is actually a fixed header; setup events
function setupFooter(numberOfTokens) {
	$('#footer').append([
						'<small><small><button id="closeDialogs">Close all dialogs</button>',
						'<button id="generatedNodes">Toggle generated nodes</button>',
						'<button id="expandedNodes">Toggle timeline/expanded</button>',
						'<button id="optionsBox">Toggle options box</button>',
						'<button id="reset">Resize</button>',
						'<button id="defaultvals">Reset to default</button>',
						'<button id="customNodes">Hide specific nodes</button></small></small>',
						//'<button id="closeWindow">Close window</button>'
						].join(''));
	$('#closeDialogs').button();
	$('#generatedNodes').button();
	$('#expandedNodes').button();
	$('#optionsBox').button();
	$('#reset').button();
	$('#defaultvals').button();
	$('#customNodes').button();
	$('#footer').append([
						 '<a href="http://sourceforge.net/apps/mediawiki/plexil/index.php?title=Viewing_Plan_Execution#Temporal_Plan_Viewer" id="help" target="blank">',
						 'Help',
						 '</a>'
						 ].join(''));
	$('#closeDialogs').click(function() {
								for(var i = 0; i < numberOfTokens; i++) {
									$('#dialogBox'+i).dialog("close");
								}
								});
	$('#generatedNodes').click(function() {
										toggleCookiesGenerated();
										});
	$('#expandedNodes').click(function() {
										toggleCookiesExpanded();
										});
	$('#reset').click(function() {
										window.location.reload();
										});
	$('#optionsBox').click(function() {
									if($('#mod').css('display') == 'none') {
										$('#mod').show();
										$('#gantt').css('top','195px');
									}
									else {
										$('#mod').css('display','none');
										$('#gantt').css('top','25px');
									}
									});
	$('#defaultvals').click(function() {
								 deleteAllCookies();
								 window.location.reload();
								 });
	$('#customNodes').click(function() {
									 $('#gantt').append([
														'<div id="customNodesBox">',
														'Specify nodes (separate with commas; no spaces):<br>',
														'<textarea id="customNodesText" cols="40" rows="8" /><br>',
														'<button id="customNodesButton">Submit</button>',
														'<button id="customNodesClearButton">Show all nodes</button>',
														'<button id="listHiddenNodesButton">List hidden nodes</button>',
														'</div>'
														].join(''));
									 $('#customNodesBox').dialog({
																autoOpen:false,
																height:300,
																width:460,
																title:'Hide specific nodes',
																});
									 $('#customNodesBox').dialog("open");
									 $('#customNodesButton').click(function() {
																			performCustomNodes();
																			window.location.reload();
																			});
									 $('#customNodesClearButton').click(function() {
																			deleteCookie("showCustomCookie");
																			window.location.reload();
																			});
									 $('#customNodesText').val(getCookie("showCustomCookie"));
									 $('#listHiddenNodesButton').click(function() {	
															$('#customNodesUnhideBox').dialog("open");
																				});
									 $('#customNodesBox').append('<div id="customNodesUnhideArea"></div>');
									 }); 
									
	//only supported in Safari
	$('#closeWindow').click(function() {
									 deleteAllCookies();
									 window.close();
									 });
														 

}

//get a global copy of the tokens
var myTokenSet = new Array();
function keepTokens(tokens) {
	var i = 0;
	for(i = 0; i < tokens.length; i++) {
		myTokenSet[i] = tokens[i];
	}
}

//display dialogs
function displayDialogBox(event) {
	var token = $(event.target).data("token");
	var myidentifier = 0;
	var i = 0;
	for(i = 0; i < myTokenSet.length; i++) {
		if(myTokenSet[i] == token) {
			myidentifier = i;
		}
	}
	$('#dialogBox'+myidentifier).dialog("open");
	//could add cool effects for dialog boxes - see jquery documentation
	//$('#dialogBox'+myidentifier).dialog( { hide: 'clip' } );
	//$('#dialogBox'+myidentifier).dialog( { show: 'slide' } );
	addSelectionStyling(event.target);
}

//to assess the number of iterations a looping nodes goes through
var myLoops = [];
function loopTracker(tokens) {
	var thisId;
	var currentValue;
	for(var i = 0; i < tokens.length; i++) {
		thisId = parseFloat(tokens[i].id);
		if(myLoops[thisId] > 0) {
			currentValue = myLoops[thisId];
			myLoops[thisId] = currentValue + 1;
		}
		else {
			myLoops[thisId] = 0;
			currentValue = myLoops[thisId];
			myLoops[thisId] = currentValue + 1;
		}
	}
}

/** cookie togglers  (switch value back and forth) **/

function toggleCookiesGenerated() {
	var temp = getCookie("showGenCookie");
	if (temp == "true")
  	{
  		setCookie("showGenCookie","false",365);
  	}
	else 
  	{
    	setCookie("showGenCookie","true",365);
	}
	window.location.reload();
}

function toggleCookiesExpanded() {
	var temp = getCookie("showLineCookie");
	if (temp == "true")
  	{
  		setCookie("showLineCookie","false",365);
  	}
	else 
  	{
    	setCookie("showLineCookie","true",365);
    }
	window.location.reload();
}

/** handling specific nodes to hide **/
var customNodesArray = new Array();
var customNodesUnhideArray = new Array();

/** repack the user's input to CSV string to package into cookie **/
function performCustomNodes() {
	var getVal = $('#customNodesText').val();
	var copy = getVal;
	var final;
	customNodesArray = unpackCSVString(copy);
	final = packCSVString(customNodesArray);
	storeCustomNodesCookie(final);
}

/** package an array into CSV string
 * @param array
 * @param string
**/
function packCSVString(valArray) {
	var string = "";
	for(var i = 0; i < valArray.length; i++) {
		string = string + valArray[i] + ',';
	}
	string = string.substring(0, string.length-1);
	return string;
}

/** unpackage CSV string into an array 
 * @param string
 * @return array
**/
function unpackCSVString(copy) {
	var valArray = new Array();
	var commas = new Array();
	var start = 0;
	
	for(var i = 0; i < copy.length; i++) {
		if(copy.charAt(i) == ',') commas.push(i);
	}
	for(var i = 0; i < commas.length+1; i++) {
		valArray[i] = copy.substring(start,commas[i]);
		if(copy.charAt(commas[i]+1) == " ") start = commas[i]+2;
		else start = commas[i]+1;
	}
	return valArray;
}

//set showCustomCookie to CSV string of specific nodes
function storeCustomNodesCookie(final) {
	setCookie("showCustomCookie", final, 365);
}