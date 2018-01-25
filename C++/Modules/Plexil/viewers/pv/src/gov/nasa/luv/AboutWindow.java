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

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
//import java.awt.Dialog.ModalityType;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.BorderFactory;
import javax.swing.Icon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import javax.swing.WindowConstants;

public class AboutWindow
    extends JFrame {

    private static AboutWindow _the_instance_ = null;

    public static AboutWindow instance() {
        if (_the_instance_ == null)
            _the_instance_ = new AboutWindow();
        return _the_instance_;
    }

    private AboutWindow() {
        super();
        setDefaultCloseOperation(WindowConstants.HIDE_ON_CLOSE);
        setTitle("About Plexil Viewer");
        setBackground(Color.WHITE);
        setAlwaysOnTop(true);

        JPanel aboutPane = new JPanel(new BorderLayout(), false);
        aboutPane.setBackground(Color.WHITE);

        Icon logo = Constants.loadImage(Constants.START_SCREEN_ICO);
        aboutPane.add(new JLabel(logo, SwingConstants.CENTER),
                      BorderLayout.NORTH);

        JLabel txt =
            new JLabel("<html><center><font size=+1>Plan Execution Interchange Language</font><br>" 
                       + Constants.PLEXIL_VERSION + "<br><br>"
                       + Constants.PLEXIL_WEBSITE + "<br><br>"
                       + Constants.PLEXIL_COPYRIGHT + "<br></center></html>",
                       SwingConstants.CENTER);
        txt.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
        aboutPane.add(txt, BorderLayout.CENTER);

        setContentPane(aboutPane);
        pack();
        setLocationRelativeTo(null);
    }

}
