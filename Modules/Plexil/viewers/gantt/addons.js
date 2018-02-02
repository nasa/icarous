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
*/

// JavaScript Document
var myTokenFileName = "../rawPlanTokens.js";
var showGeneratedNodes = provideGenCookie();
var showExpanded = provideLineCookie();

//to show the plan currently being viewed
var numtimes = 0;
$(document).ready(callReadyFunc);
function callReadyFunc() { 
	if(numtimes < 1) {
		$('#mod').append(tellIfDefault + '<div="addplan"><input type="text" id="planname" class="newhidden" /><button id="planstartup" class="newhidden">Add plan</button><button id="defaultplan" class="newhidden">Latest run</button></div>');
	}
	numtimes++;
}

/** COOKIES
	* showGenCookie - boolean, toggle generated nodes
	* showLineCookie - boolean, toggle timeline/expanded
	* showFileCookie - string, file name of plan, deprecated and no longer in use
	* showPixelsCookie - parsed as int, perserve getPixelsPerTimeIncrement() - value from UI, defined in main
	* showHeightCookie - parsed as int, perserve getTokenHeight() - value from UI, defined in main
	* showScaleCookie - parsed as int, perserve getScaling() - value from UI, defined in main
	* showCustomCookie - parsed as string, specific nodes to hide, defined in detailsBox
**/

function provideGenCookie() {
	var temp = getCookie("showGenCookie");
	if(temp != null && temp != "") {
		return temp;
	}
	else {
		setCookie("showGenCookie","false",365);
		var newtemp = getCookie("showGenCookie");
		return newtemp;
	}
}

function provideLineCookie() {
	var temp = getCookie("showLineCookie");
	if(temp != null && temp != "") {
		return temp;
	}
	else {
		setCookie("showLineCookie","true",365);
		var newtemp = getCookie("showLineCookie");
		return newtemp;
	}
}

/** cookie setters, accessors, deletors **/

function setCookie(c_name,value,exdays)
{
var exdate=new Date();
exdate.setDate(exdate.getDate() + exdays);
var c_value=escape(value) + ((exdays==null) ? "" : "; expires="+exdate.toUTCString());
document.cookie=c_name + "=" + c_value;
}

function getCookie(c_name)
{
var i,x,y,ARRcookies=document.cookie.split(";");
for (i=0;i<ARRcookies.length;i++)
{
  x=ARRcookies[i].substr(0,ARRcookies[i].indexOf("="));
  y=ARRcookies[i].substr(ARRcookies[i].indexOf("=")+1);
  x=x.replace(/^\s+|\s+$/g,"");
  if (x==c_name)
    {
    return unescape(y);
    }
  }
}

function deleteCookie(name) {
document.cookie = name +
'=; expires=Thu, 01-Jan-70 00:00:01 GMT;';
}

function deleteAllCookies() {
	deleteCookie("showGenCookie");
	deleteCookie("showLineCookie");
 	deleteCookie("showFileCookie");
 	deleteCookie("showPixelsCookie");
	deleteCookie("showHeightCookie");
	deleteCookie("showScaleCookie");
	deleteCookie("showCustomCookie");
}

function initializeCustomNodesArray() {
	customNodesArray = unpackCSVString(getCookie("showCustomCookie"));
	for(var i = 0; i < customNodesArray.length; i++) {
		while(customNodesArray[i].charAt(0) == "\n") customNodesArray[i] = customNodesArray[i].substring(1);
		while(customNodesArray[i].charAt(0) == " ") customNodesArray[i] = customNodesArray[i].substring(1);
		while(customNodesArray[i].charAt(customNodesArray[i].length-1) == " ") customNodesArray[i] = customNodesArray[i].substring(0, customNodesArray[i].length-1);
	}
}

/** determines if a custom node matches a token value **/
function isCustomNode(temp, temp2, temp3) {
	var masterDecision = false;
	var isExactNode = doExactNode(temp, temp2, temp3);
	var isRegularExpression = doRegularExpression(temp,temp2,temp3);
	if(isExactNode || isRegularExpression) masterDecision = true;
	return masterDecision;
}

/** checks custom node specifications for exact matches **/
function doExactNode(temp, temp2, temp3) {
	var isCustom = false;
	if(getCookie("showCustomCookie") == null || getCookie("showCustomCookie") =='') return isCustom;
	else initializeCustomNodesArray();
	for(var i = 0; i < customNodesArray.length; i++) {
		if((customNodesArray[i].indexOf('*') == -1) && (customNodesArray[i].indexOf('+') == -1) && (customNodesArray[i].indexOf('?') == -1)) {
			if(getCookie("showLineCookie") == "false") {
				if(((temp == customNodesArray[i]) && (temp.length == customNodesArray[i].length))) 
					isCustom = true;
			}
			else {
				if(((temp2 == customNodesArray[i]) && (temp2.length == customNodesArray[i].length)) 
																				 	 //|| ((temp2 == customNodesArray[i]) && (temp2.length == customNodesArray[i].length)) 
																				  	 //|| ((temp3 == customNodesArray[i]) && (temp3.length == customNodesArray[i].length))
																				 	 )
					isCustom = true;
			}
		}
	}
	return isCustom;
}

/** checks custom node specifications for wildcard matches **/
function doRegularExpression(temp,temp2,temp3) {
	var isRegExp = false;
	for(var i = 0; i < customNodesArray.length; i++) {
		if(customNodesArray[i].indexOf('*') != -1) {
			if(getCookie("showLineCookie") == "false")
				isRegExp = handleReferenceString(customNodesArray[i], temp, temp, temp3);
			else
				isRegExp = handleReferenceString(customNodesArray[i], temp, temp2, temp3);
			if(isRegExp) return isRegExp;
		}
	}
	return isRegExp;
}

/** handles regular expression/wildcard * in custom node specifications **/
function handleReferenceString(string, temp, temp2, temp3) {
	var stars = new Array();
	var finalBool = true;
	for(var i = 0; i < string.length; i++) {
		if(string.charAt(i) == '*') {
			stars.push(i);
		}
	}
	//stars.reverse();
	//
	//for(var i = 0; i < stars.length; i++) alert(stars[i]);
	//
	var start = 0;
	//8/19/11
	var constructStrings = new Array();
	for(var i = 0; i < stars.length; i++) {
		var newstring = string.substring(start, stars[i]);
		start = stars[i]+1;
		constructStrings.push(newstring);
	}
	if(start != string.length) {
		var newstring = string.substring(start);
		constructStrings.push(newstring);
	}
	var tempBools = new Array();
	if(temp2.length < string.length) tempBools.push(false);
	for(var i = 0; i < constructStrings.length; i++) {
		if(temp2.indexOf(constructStrings[i]) != -1) {
			tempBools.push(true);
		}
		else tempBools.push(false);
	}
	for(var i = 0; i < tempBools.length; i++) {
		if(tempBools[i] == false) finalBool = false;
	}
	return finalBool;
	/**
	var boolVals = new Array();
	for(var i = 0; i < stars.length+1; i++) {
		if(string.substring(start, stars[i]) != '') {
			if((temp.indexOf(string.substring(start, stars[i])) != -1) || (temp2.indexOf(string.substring(start, stars[i])) != -1) || (temp3.indexOf(string.substring(start, stars[i])) != -1)) {
				boolVals.push(true);
			}
			else {
				boolVals.push(false);
			}
		}
		start = stars[i]+1;
	}
	for(var i = 0; i < boolVals.length; i++) {
		if(boolVals[i]) {
			finalBool = true;
		}
		else {
			finalBool = false;
		}
	}
	return finalBool;
	**/
}

function doCustomUnhide(temp, temp2) {
	var isInc = false;
	for(var i = 0; i < customNodesUnhideArray.length; i++) {
		if(customNodesUnhideArray[i] == temp2) isInc = true;
	}
	if(!isInc) customNodesUnhideArray.push(temp2);
	$('#gantt').append([
								 '<div id="customNodesUnhideBox">',
								 '<table border="0">',
								 '<tr>',
								 '<td width="150"><strong>Node name</td>',
								 //'<td><strong>Unhide</td>',
								 '</tr>',
								 ].join(''));
	for(var i = 0; i < customNodesUnhideArray.length; i++) {
		$('#customNodesUnhideBox').append([
									 '<tr>',
									 '<td>&nbsp;',
									 customNodesUnhideArray[i],
									 '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;',
									 '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>',
									 //'<td><input type="checkbox" value="',
									 //customNodesUnhideArray[i],
									 //'"/></td>',
									 '</tr>',
									 ].join(''));
	}
	$('#customNodesUnhideBox').append([
								 '</table>',
								 '</div>',
								 ].join(''));
	/**
	$('#customNodesUnhideBox').append([
									   '<button id="customNodesUnhideButton">',
									   'Unhide selected',
									   '</button>',
									   ].join(''));
	**/
	$('#customNodesUnhideBox').dialog({
								autoOpen:false,
								height:250,
								width:400,
								title:'Hidden nodes',
								});
	$('#customNodesUnhideBox').dialog("close");
	//$(":checkbox").click(keepUnhideChecks);
	/**
	$('#customNodesUnhideButton').click(function() {
												 finishUnhideChecks();
												 });
	**/
}

/**
var customUnhideChecks = new Array();
function keepUnhideChecks() {
	var n = $("input:checkbox:checked").val();
	customUnhideChecks.push(n);
}

function finishUnhideChecks() {
	var cookie = getCookie("showCustomCookie");
	for(var i = 0; i < customUnhideChecks.length; i++) {
		//alert(customUnhideChecks[i]);
		cookie = cookie.substring(0, cookie.indexOf(customUnhideChecks[i])) + cookie.substring(cookie.indexOf(customUnhideChecks[i])+customUnhideChecks[i].length+1);
	}
	setCookie("showCustomCookie",cookie);
	//alertonce(getCookie("showCustomCookie"));
}
**/

/** alerters for debugging **/
var alerter = 0;
function alertonce(string) {
	if(alerter == 0) {
		alert(string);
	}
	alerter++;
}