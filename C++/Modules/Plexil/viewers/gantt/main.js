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

$(document).append([
					'<div id="loading">',
					'Loading',
					'</div>'
					].join(''));

$(window).load(function(){  
      $("#loading").hide();  
});  

$(document).ready(getSetup);

//initialize cookies and organize and clean divs
function getSetup() {
	if(getCookie("showPixelsCookie") == null || getCookie("showPixelsCookie") == "")
		setCookie("showPixelsCookie",20,365);
	if(getCookie("showHeightCookie") == null || getCookie("showHeightCookie") == "")
		setCookie("showHeightCookie",15,365);
	if(getCookie("showScaleCookie") == null || getCookie("showScaleCookie") == "")
		setCookie("showScaleCookie",1,365);
	addModBox();
	$('#gantt').empty();
	$('#footer').empty();
	startUp(); 
    $('#target').submit(function() {
		getDisplayCookies();
		$('#gantt').empty();
		$('#footer').empty();
		//window.location.search = $('#browseTokenFileName').val();
		startUp(); 
		return false;
    });
}

//build grid top down
function startUp() {
	var allTokens = getAndConvertTokens();
	showTokens(allTokens);
	drawGrid(); 
	attachEvents();
}

//add click events to token lines to display boxes
function attachEvents() {
	$(".planLine").click(displayDialogBox);
	$(".simLine").click(displayDialogBox);
}

//specify the file name
var tellIfDefault = checkIfDefaultFile();
function checkIfDefaultFile() {
	return "Currently using " + myHTMLFilePathString;
}

//build options box
function addModBox() {
   $("#mod").append([
	'<form id="target">',
		'<label for="PixelsPerTimeIncrementAmount">Pixels per time increment:</label>',
		'<input type="text" id = "PixelsPerTimeIncrementAmount" style="border:0;background-color:gainsboro"/>',
    	'<div id="PixelsPerTimeIncrementSlider"></div>',
       	'<label for="TokenHeightAmount">Token height:</label>',
		'<input type="text" id = "TokenHeightAmount" style="border:0;background-color:gainsboro;" />',
       	'<div id="TokenHeightSlider"></div>',
	  	'<div id="scaleradio">',
			'<label for="scaleradio">Scale down: </label>',
			'<small>',
			'<input type="radio" id="scale1" value="1000" name="radio" /><label for="scale1">0.001x</label>',
			'<input type="radio" id="scale2" value="100" name="radio" /><label for="scale2">0.01x</label>',
			'<input type="radio" id="scale3" value="10" name="radio" /><label for="scale3">0.1x</label>',
			'<input type="radio" id="scale4" value="1" name="radio" checked="checked" /><label for="scale4">1x</label>',
			'</small>',
		'</div>',
	   	'<small><button id="ModBoxButton">Update Display</button></small>',
	'</form>'
    ].join(''));
   $('#ModBoxButton').button();
    //shows how to modify css display characteristics, use for details boxes
    $('#submitButton').css('display','');
    buildPixelsPerTimeIncrementSlider();
    buildTokenHeightSlider();
	buildScaleRadio();
	buildSubmitButton();
}

//get cookies from options box
function getDisplayCookies() {
	var pixelsliderval = $('#PixelsPerTimeIncrementSlider').slider("value");
	var heightslidervalval = $('#TokenHeightSlider').slider("value");
	var scaleradioval = $(":checked").val();
	setCookie("showHeightCookie",heightslidervalval,365);
	setCookie("showPixelsCookie",pixelsliderval,365);
	setCookie("showScaleCookie",scaleradioval,365);
}

/** get values from the sliders and set cookies to their values **/

//pixels per time increment is the spacing between the grid lines
function getPixelsPerTimeIncrement() {
	var temp = getCookie("showPixelsCookie");
	var tempval = $('#PixelsPerTimeIncrementSlider').slider("value");
	if(temp != null && temp != "") {
		return parseInt(temp);
	}
	else {
		setCookie("showPixelsCookie",tempval,365);
		var newtemp = getCookie("showPixelsCookie");
		return parseInt(newtemp);
	}
}

//token height is the spacing in pixels between token lines
function getTokenHeight() {
	var temp = getCookie("showHeightCookie");
	var tempval = $('#TokenHeightSlider').slider("value");
	if(temp != null && temp != "") {
		return parseInt(temp);
	}
	else {
		setCookie("showHeightCookie",tempval,365);
		var newtemp = getCookie("showHeightCookie");
		return parseInt(newtemp);
	}
}

//scaling is the zoom in powers of ten; the values that the grid lines represent
function getScaling() {
	var temp = getCookie("showScaleCookie");
	var tempval = $(":checked").val();
	if(temp != null && temp != "") {
		return parseInt(temp);
	}
	else {
		setCookie("showScaleCookie",tempval,365);
		var newtemp = getCookie("showScaleCookie");
		return parseInt(newtemp);
	}
}

/** build the options box **/

//jquery slider setup
function buildPixelsPerTimeIncrementSlider() {
    $("#PixelsPerTimeIncrementSlider").slider({
	value: getPixelsPerTimeIncrementForSlider(),
	min: 2,
	max: 100,
	step: 2,
	slide: function(event, ui) {
	    $("#PixelsPerTimeIncrementAmount").val(ui.value);
	}
    });
    $("#PixelsPerTimeIncrementAmount").val($("#PixelsPerTimeIncrementSlider").slider("value"));
}

//jquery slider setup
function buildTokenHeightSlider() {
    $("#TokenHeightSlider").slider({
	value: getTokenHeightForSlider(),
	min: 5,
	max: 50,
	step: 5,
	slide: function(event, ui) {
	    $("#TokenHeightAmount").val(ui.value);
	}
    });
    $("#TokenHeightAmount").val($("#TokenHeightSlider").slider("value"));
}

//jquery radio setup
function buildScaleRadio() {
	$("#scaleradio").buttonset();
}

//jquery button setup
function buildSubmitButton() {
	$("button","#ModBoxButton").button();
}

//get cookie value showPixelsCookie for slider
function getPixelsPerTimeIncrementForSlider() {
	if(getCookie("showPixelsCookie") == null || getCookie("showPixelsCookie") == "")
		return 20;
	else return getCookie("showPixelsCookie");
}

//get cookie value showHeightCookie for slider
function getTokenHeightForSlider() {
	if(getCookie("showHeightCookie") == null || getCookie("showHeightCookie") == "")
		return 15;
	else return getCookie("showHeightCookie");
}