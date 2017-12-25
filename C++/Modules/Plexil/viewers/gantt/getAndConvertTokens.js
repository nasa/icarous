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

// to get data from the test files, always set to true
var GET_DATA_FROM_FILES = true;

function getRawTokens(tokenSet) {
    var rawTokens;				// holds array of raw token objects in json format, wherever it came from
    var rawTokensFromJava;		// will hold a string of rawTokens in json
    
    // get the rawTokens objects from somewhere
    if (GET_DATA_FROM_FILES) {
	switch(tokenSet) {
	case "plan":
	    rawTokens = rawPlanTokensFromFile;	// already an array
	    break;
	case "sim":
	    rawTokens = rawPlanTokensFromFile;	// already an array
	    break;
	}
    } else {
	switch(tokenSet) {
	case "plan":
	    rawTokensFromJava = getPlanTokenDataExternal("give me data");
	    break;
	case "sim":
	    rawTokensFromJava = getSimulationTokenDataExternal("give me data");
	    break;
	}
	eval("rawTokens = " + rawTokensFromJava + ";");	// convert to an array by eval	
    }
    return rawTokens; 
}

/*
 * Get raw plan and sim tokens from IMDE or files, and convert to new objects.
 * Return array of all plan and sim tokens.
 */
function getAndConvertTokens() {
    var rawPlanTokens = [];
    var rawSimTokens  = [];
    var planTokens = [];	// 
    var simTokens  = [];	// 
    var allTokens  = [];	// 	
    var rawToken;			// 
    var token;				//
    var i;	

    // get and convert tokens
    rawPlanTokens	= getRawTokens("plan");
    rawSimTokens	= getRawTokens("sim");
    planTokens		= convertTokens(rawPlanTokens);
    simTokens		= convertTokens(rawSimTokens);
    
    // distinguish plan versus sim tokens by adding role
    for (i = 0; i < planTokens.length; i++) {
	planTokens[i].role = "plan";
    }	
    for (i = 0; i < simTokens.length; i++) {
	simTokens[i].role = "sim";
    }

    // Group together
    allTokens = planTokens;
    //allTokens = planTokens.concat(simTokens);
    return allTokens;
}

function convertTokens(rawTokens) {
    var result = [];
    var rawToken;
    var token;
    for (var i = 0; i < rawTokens.length; i++) {
	rawToken = rawTokens[i];
	if (rawToken.type !== "consume") { // ignore these battery/reservoir tokens for now
	    token = {
		"rawToken": 			rawToken,
		"role": 			"", // will be plan or sim
		"id": 				getTokenId(rawToken),
		"classNames": 			getClassNames(rawToken),
		"objectName": 			getObjectName(rawToken),
		"predicateName": 		getPredicateName(rawToken),
		"predicateInstanceName": 	getPredicateInstanceName(rawToken),
		"startDomain":  		getRawParameter(rawToken, "start").value,
		"durationDomain": 		getRawParameter(rawToken, "duration").value,
		"endDomain": 			getRawParameter(rawToken, "end").value,
                // e.g., [{name: "fuel", value: 10}, {name: "battery", value: 1.0}]
		"domainParameters": 		getLocalParams(rawToken),  
		// for computations use integers
		"start": 		getPosMinIntFromDomain(getRawParameter(rawToken, "start").value),
		"end":			getPosMinIntFromDomain(getRawParameter(rawToken, "end").value)
	    }
	    //filter out generated nodes
	    var temp = token.objectName;
	    var temp2 = token.predicateName;
	    var temp3 = token.predicateInstanceName;
	    if(showGeneratedNodes == "false")
	    {
		if((temp.indexOf("ep2cp_") == -1) && (temp2.indexOf("ep2cp_") == -1) && (temp3.indexOf("ep2cp_") == -1))
		{
		    if((temp.indexOf("plexilisp_") == -1) && (temp2.indexOf("plexilisp_") == -1) && (temp3.indexOf("plexilisp_") == -1))
		    {
			if((temp.indexOf("__CHILD__") == -1) && (temp2.indexOf("__CHILD__") == -1) && temp3.indexOf("__CHILD__"))
			{
			    if(!isCustomNode(temp, temp2, temp3)) {
				result.push(token);
			    }
			    else {
				doCustomUnhide(temp, temp2);
			    }
			}
		    }
		}
	    }
	    else
	    {
		if(!isCustomNode(temp, temp2, temp3)) {
		    result.push(token);
		}
		else {
		    doCustomUnhide(temp, temp2);
		}
	    }
	}		
    }
    return result;	
}

/*
 * Returns array of raw parameter objects that are domain defined
 */
function getLocalParams(rawToken) {
    var parametersToIgnore = ["entityName", "full type", "state", "object", "duration", "start", "end"];
    var allParams = rawToken.parameters;
    var parametersToShow = [];

    // create list of parameters to show
    for (var i = 0; i < allParams.length; i++) {
	for (var j = 0; j < parametersToIgnore.length; j++) {
	    if (allParams[i].name === parametersToIgnore[j]) {
		break;
	    }
	    if (j === (parametersToIgnore.length - 1)) {
		parametersToShow.push(allParams[i]);
	    }		
	}
    }
    return parametersToShow;	
}

function getClassNames(rawToken) {
    // for now, just return "full type" as a first array element
    return [getRawParameter(rawToken, 'full type').value];
}

// Get name and convert from ~ OBJECT:controller.spacecraft(86) to controller.spacecraft
function getObjectName(rawToken) {
    var objectName;
    objectName = getRawParameter(rawToken, 'object').value;
    objectName = objectName.replace(/OBJECT:/, "");
    objectName = objectName.replace(/\(\d+\)/, "");	
    return objectName;
}

function getTokenId(rawToken) {
    return parseInt(rawToken.id, 10);
}

function getPredicateName(rawToken) {
    return String(rawToken.type);
}

function getPredicateInstanceName(rawToken) {
    // old var name = String(getValueAsString(rawToken, 'entityName'));
    var name = getRawParameter(rawToken, 'entityName').value;
    // if it's a generic predicate instance name (e.g., Attitude.pointAwayFromEarth), change to " "
    if (name.search(/\./) !== -1) { //  found a "." in name
	name = " ";
    }
    return name;
}

function getRawParameter(rawToken, parameterName) {
    var result;	// return an object for the parameter with the given name
    var parameters = rawToken.parameters;		// the array of parameters for this token

    for (var i = 0; i < parameters.length; i++) {
	if (parameters[i].name === parameterName) {
	    result = parameters[i];
	    break;
	}
	if (i === (parameters.length - 1)) {
	    alert(name + " parameter not found in token id: " + rawToken.id);			
	}
    }
    return result;	
}

function getPosMinIntFromDomain(domainString) {
    // domainString may be something like "0", "[2.0, 30.0]", or "[5, +inf]"
    // For above inputs, return 0, or 2, or 5
    var result;
    var partial; // used to hold the string after the "[" 
    
    if (domainString.charAt(0) === "["){
	partial = domainString.slice(1);
	result = parseInt(partial, 10);
    } else {
	result = parseInt(domainString, 10);
    }

    if (isNaN(result) || (result < 0)) {
	alert("Got a unexpected domain minimum: ", domainString);
    }
    return result;
}

/*
 * Converts infinities if needed, otherwise returns the input domain string.
 * E.g., converts [-4.503599627370495E15, 4.503599627370495E15] to [-inf, +inf]
 */
function convertInfinities(domainString) {
    var negInf = -1E15; // based on what's coming back from Java IMDE
    var posInf =  1E15;
    var minDomain, maxDomain;
    var partial;
    
    if (domainString.charAt(0) === "["){
	// convert min
	partial = domainString.slice(1);
	minDomain = parseFloat(partial);
	if (minDomain < negInf) minDomain = "-inf";
	// convert max
	partial = domainString.substr((domainString.indexOf(',') + 1), (domainString.length - 1));
	maxDomain = parseFloat(partial);
	if (maxDomain > posInf) maxDomain = "+inf";		
	return "[" + minDomain + ", " + maxDomain + "]";
    } else {
	return domainString;
    }
}


