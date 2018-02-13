/* Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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
 */

package gov.nasa.luv;

import static gov.nasa.luv.Constants.PROP_ARRAY_MAX_CHARS;

import java.util.Iterator;
import java.util.Vector;

public class ArrayVariable
    extends Variable {

	private Vector<Variable> list;
	
	public ArrayVariable(String inOut, String name, String type, String value){
		super(inOut, name, type, value);
		String[] valueList = null;
    	int nvalues = 0;
    	if (value != null) {        		
            valueList = value.replaceAll(",", "").split(" "); // ???
            nvalues = valueList.length;
        }
    	String array_name = name.replaceFirst("\\[.*", "");
    	Integer len = Integer.parseInt(name.replaceAll("[^0-9]", ""));
    	list = new Vector<Variable>(len);
    	for (int i = 0; i < len; i++) {
            if (nvalues > 0) {
                list.add(new Variable("--", array_name + "[" + i + "]", type, valueList[i]));
                nvalues--;
            }
            else
                list.add(new Variable("--", array_name + "[" + i + "]", type, "U" ));
        }
	}

	public void setVariable(Integer index, String value)
	{
		list.get(index).setValue(value);
	}
	
	public Variable getVariable(Integer index)
	{
		return list.get(index);
	}

    public Vector<Variable> getVariables() {
        return list;
    }
	
	public String getBaseName() {
		return getBaseName(getName());
	}
	
	public static String getBaseName(String input) {
        int bracket = input.indexOf('[');
        if (bracket < 0)
            return input;
        else
            return input.substring(0, bracket);
	}

    public static int getIndex(String input) {
        int lbracket = input.indexOf('[');
        if (lbracket < 0)
            return -1; // not found
        int rbracket = input.indexOf(']', lbracket + 1);
        if (rbracket < 0)
            return -1; // ill-formed
        String num = input.substring(lbracket + 1, rbracket);
        if (num.isEmpty())
            return -1; // nothing between brackets
        else
            return Integer.parseInt(num);
    }

	public void setArrayIndexVariable(int index, String value) {
        if (index < 0)
            return;
        setVariable(index, value);
		updateList();
	}
	
	public int size() {
		return list.size();
	}
	
	public void updateList(){
		int length = 0;
		String arrayValue = "#(";
		Iterator<Variable> it = list.iterator();
		while(it.hasNext() && length < Settings.instance().getInteger(PROP_ARRAY_MAX_CHARS))
            {			
                arrayValue+=it.next().getValue();
                if(it.hasNext())
                    {				
                        arrayValue+=", ";				
                    }
                length = arrayValue.length();
            }
		if (length >= Settings.instance().getInteger(PROP_ARRAY_MAX_CHARS))
			arrayValue+="...";
		arrayValue+=")";
		setValue(arrayValue);
	}
	
}
