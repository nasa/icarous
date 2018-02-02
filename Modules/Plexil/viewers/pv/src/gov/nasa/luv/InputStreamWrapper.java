/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
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

import java.io.InputStream;
import java.io.IOException;
import java.io.EOFException;

import static gov.nasa.luv.Constants.END_OF_MESSAGE;

/**
 * The InputStreamWrapper class provides methods for processing an input stream
 * and simulates end of file when it reaches an END_OF_MESSAGE marker.
 */ 

public class InputStreamWrapper extends InputStream
{
    // the stream being wrapped
    private InputStream str;

    // true if str has reached EOF, false otherwise
    // @note when true, buffer will be empty
    private boolean strEOF;

    // local buffer
    private byte[] buffer;
    private int bufferRead;
    private int bufferWrite;

    // tracks max bytes used in buffer
    private int highWaterMark;

    /**
     * Constructs an InputStreamWrapper with the specified InputStream. 
     * 
     * @param s the input stream on which the InputStreamWrapper operates
     */
    
    InputStreamWrapper(InputStream s)
    {
	str = s;
	strEOF = false;

	// initialize internal buffer
	buffer = new byte[1024]; // size is arbitrary
	bufferRead = bufferWrite = highWaterMark = 0;
    }

    //
    // Buffer management
    //

    // Return # of bytes currently in buffer before END_OF_MESSAGE.
    private int locallyAvailable()
    {
	int i = bufferRead;
	while (i < bufferWrite && buffer[i] != END_OF_MESSAGE)
	    i++;
	return i - bufferRead;
    }

    // Gets data from the wrapped stream into the buffer, blocking if necessary.
    //  Sets strEOF = true when it reaches EOF on the wrapped stream.
    // @note Call only when buffer is empty.
    private void fillBuffer() throws IOException
    {
	if (strEOF) {
	    return;
	}

	bufferRead = 0;
	bufferWrite = str.read(buffer); // returns -1 @ EOF

	// if we got 0 bytes, do it again - 
	// next read should return EOF
	if (bufferWrite == 0)
	    bufferWrite = str.read(buffer);

	if (bufferWrite == -1) {
	    // handle EOF
	    bufferWrite = 0;
	    strEOF = true;
	}
	if (bufferWrite > highWaterMark)
	    highWaterMark = bufferWrite;
    }

    //
    // Public InputStream API
    //

    /**
     * This method does nothing in order to preserve state across calls 
     * to XMLReader.parse().
     */ 
    public void close() throws IOException
    {
    }

    /**
     * Returns the integer amount of what avaible to read in the buffer.
     * @return the integer amount of what avaible to read in the buffer.
     * @throws java.io.IOException
     */
    public int available() throws IOException
    {
	int locally = locallyAvailable();
	if (locally != 0
	    || bufferRead != bufferWrite) {
	    return locally;
	}

	// at this point we know buffer is empty
	if (str.available() == 0) {
	    return 0;
	}

	// we can presume this won't block
	// (but it could throw an exception)
	fillBuffer();
	locally = locallyAvailable();
	return locally; // can return 0!
    }

    /**
     * Reads the buffer.
     * @return an EOF indicator or the next integer in the buffer
     * @throws java.io.IOException
     */
    public int read() throws IOException
    {
	if (bufferRead == bufferWrite)
	    fillBuffer(); // can block, throw exception, hit EOF
	if (strEOF) {
	    throw new EOFException("End of file on wrapped stream");
	}

	// below here, can assume buffer non-empty
	int result = buffer[bufferRead++];
	if (result == END_OF_MESSAGE) {
	    return -1; // end of file
	}
	return result;
    }

    /**
     * Returns the result of the read(byte[] b, int off, int len) method.
     * @param b
     * @return the result of the read(byte[] b, int off, int len) method
     * @throws java.io.IOException
     */
    public int read(byte[] b) throws IOException
    {
	return read(b, 0, b.length);
    }

    /**
     * Reads the buffer.
     * @param b buffer
     * @param off offset
     * @param len length
     * @return an EOF indicator or the next index
     * @throws java.io.IOException
     */
    public int read(byte[] b, int off, int len) throws IOException
    {
	if (bufferRead == bufferWrite)
	    fillBuffer(); // can block
	if (strEOF) {
	    throw new EOFException("End of file on wrapped stream");
	}

	// below here, can assume buffer non-empty
	int i = 0;
	byte thisByte;
	while (bufferRead < bufferWrite
	       && (thisByte = buffer[bufferRead]) != END_OF_MESSAGE) {
	    b[off++] = thisByte;
	    i++;
	    bufferRead++;
	}
	if (i == 0) {
	    // EOM was first byte in buffer.
	    // Bump the read pointer past it.
	    bufferRead++;
	    return -1;
	}
	return i;
    }
}
