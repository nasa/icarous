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

import javax.swing.AbstractAction;
import java.awt.event.ActionEvent;
import javax.swing.KeyStroke;
import static gov.nasa.luv.Constants.*;

/**
 * The LuvAction class provides a standard class from which to subclass Luv actions.
 */

public abstract class LuvAction extends AbstractAction
{
    private KeyStroke accelerator;

    /**
     * Constructs a LuvAction with a given name, shortcut key and
     * description.  This version assumes no accelerator key.
     *
     * @param name        name of this action
     * @param description description of this action
     */
      
    public LuvAction(String name, String description)
    {
        super(name);
        putValue(SHORT_DESCRIPTION, description);
    }

    /**
     * Constructs a LuvAction with a given name, shortcut key and
     * description.  This version assumes no key modifiers for
     * shortcut.
     *
     * @param name        name of this action
     * @param description description of this action
     * @param keyCode     identifies key for this action
     */
      
    public LuvAction(String name, String description, int keyCode)
    {
        super(name);
        putValue(SHORT_DESCRIPTION, description);
        accelerator = KeyStroke.getKeyStroke(keyCode, 0);
        putValue(ACCELERATOR_KEY, accelerator);
    }
      
    /**
     * Constructs a LuvAction with a given name, shortcut key
     * and description.
     *
     * @param name        name of this action
     * @param description description of this action
     * @param keyCode     identifies shortcut key for this action
     * @param modifiers   modifiers for shortcut (SHIFT, META, etc.)
     */
      
    public LuvAction(String name, String description,
                     int keyCode, int modifiers)
    {
        super(name);
        putValue(SHORT_DESCRIPTION, description);
        accelerator = KeyStroke.getKeyStroke(keyCode, modifiers);
        putValue(ACCELERATOR_KEY, accelerator);
    }

    /** Gets text name of the accelerator key for this action. 
     *
     * @return some random description of the accelerator key
     */
    public String getAcceleratorDescription() {
        // if there is no accelerator indicate this
        if (accelerator == null)
            return "<no key>";

        // get and clean up the key name
        // Syntax is
        //  <modifiers>* (<typedID> | <pressedReleasedID>)
        // where
        //  modifiers := shift | control | ctrl | meta | alt | altGraph
        //  typedID := typed <typedKey>
        //  typedKey := string of length 1 giving Unicode character.
        //  pressedReleasedID := (pressed | released) key
        //  key := KeyEvent key code name, i.e. the name following "VK_".

        String key = accelerator.toString();
        key = key.replaceFirst("(?:typ|press|releas)ed ", "").trim();
        key = key.replace("  ", " ");
        key = key.replace(" ", "-");
        return key;
    }

    /**
     * Called when the given action is to be executed. This
     * function must be implemented by the subclass.
     *
     * @param  e action event 
     */
      
    abstract public void actionPerformed(ActionEvent e);
}
