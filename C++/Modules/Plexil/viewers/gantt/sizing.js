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

// Based on number of major grid lines in the upper grid/number header, size the overall token column.
// Currently called from within grid.js

function sizeTokenColumn () {
	var majorGridLinesCount = $("div.gridHeader div.majorGridLine").length;
    var stepWidth = getPixelsPerTimeIncrement();
	var tokenColumn = $("div.tokenColumn");
	var width = majorGridLinesCount * 10 * stepWidth;
	var leftOffset = GRID_SETTINGS.objectColumnWidth + GRID_SETTINGS.columnBuffer;
	
	// set token column width and left offset
	//width = $(document).width() - 228; //228 takes into account the size required for the left object column
	width = $(window).width() - 150 - 28;
	console.log(leftOffset);
	tokenColumn.css("width", width);
	tokenColumn.css("left", leftOffset); 
	
	// also size the grid header
	$(".gridHeader").css("width", width);
	$(".gridHeader").css("left", '0px');
}

function sizeObjectColumn () {
	var objectColumn = $("div.objectColumn");
	objectColumn.css("width", GRID_SETTINGS.objectColumnWidth);
}

function sizeGantt () {
	var width =
		parseInt($("div.objectColumn").css("width"), 10) +
		GRID_SETTINGS.columnBuffer +
		parseInt($("div.tokenColumn").css("width"), 10);
	$("#gantt").css("width", width);
}
