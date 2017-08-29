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

import java.io.IOException;
import java.io.OutputStream;
import javax.swing.JTextArea;



public class TextAreaOutputStream
    extends OutputStream {

    private JTextArea textArea;
    private static int BUFSIZE = 1024;
    private byte[] buffer;
    private int bufptr = 0;

    public TextAreaOutputStream(JTextArea t) {
        super();
        textArea = t;
        buffer = new byte[BUFSIZE];
    }

    @Override
    public void write(int b)
        throws IOException {
        write(new byte[] {(byte) b}, 0, 1);
    }

    @Override
    public void write(byte[] b) 
        throws IOException {
        write(b, 0, b.length);
    }

    // General case.
    @Override
    public void write(byte[] b, int off, int len) 
        throws IOException {
        if (bufptr + len <= BUFSIZE) {
            // easy case
            System.arraycopy(b, off, buffer, bufptr, len);
            bufptr += len;
        }
        else 
            while (len > 0) {
                int copylen =
                    (bufptr + len > BUFSIZE)
                    ? BUFSIZE - bufptr
                    : len;                
                System.arraycopy(b, off, buffer, bufptr, copylen);
                off += copylen;
                bufptr += copylen;
                len -= copylen;
                if (len > 0)
                    flush(); // buffer is full, flush and continue
            }

        // If buffer is full after write, flush it now.
        if (bufptr == BUFSIZE)
            flush();
    }

    @Override
    public void flush()
        throws IOException {
        textArea.append(new String(buffer, 0, bufptr));
        textArea.setCaretPosition(textArea.getText().length());
        bufptr = 0;
    }

    @Override
    public void close()
        throws IOException {
        flush();
    }
}
                                    

