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
import java.awt.Component;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.io.File;
import java.io.IOException;
import java.util.Vector;
import java.util.Collection;
import java.util.List;
import javax.swing.DefaultListModel;
import javax.swing.JComponent;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.ListCellRenderer;
import javax.swing.TransferHandler;

import static java.awt.Color.*;
import static java.awt.datatransfer.DataFlavor.*;
import static javax.swing.ScrollPaneConstants.*;
import static javax.swing.ListSelectionModel.SINGLE_SELECTION;
import static javax.swing.TransferHandler.*;

/**
 * @class FileListPanel
 * @brief A user interface element for displaying, creating, and modifying
 * a list of File instances. Intended for use as an element of larger GUIs.
 */

public class FileListPanel
    extends JPanel {
    private JList<File> fileListView;
    private DefaultListModel<File> model;

    // For drag & drop support
    private static DataFlavor fileDataFlavor = null;

    public FileListPanel() {
        super(new BorderLayout());

        // Set static variable on first call
        if (fileDataFlavor == null)
            try {
                fileDataFlavor = new DataFlavor(Class.forName("java.io.File"), "file"); 
            } catch (Exception e) {
                // do nothing - if it happens things are badly broken
            }

        model = new DefaultListModel<File>();
        fileListView = new JList<File>(model);
        fileListView.setCellRenderer(new CellRenderer());
        fileListView.setSelectionMode(SINGLE_SELECTION); // K.I.S.S.
        fileListView.setTransferHandler(new FileListTransferHandler());
        // TODO?: more list setup

        JScrollPane scroller =
            new JScrollPane(fileListView,
                            VERTICAL_SCROLLBAR_AS_NEEDED,
                            HORIZONTAL_SCROLLBAR_AS_NEEDED);
        add(scroller, BorderLayout.CENTER);
    }

    public void setFiles(Collection<File> files) {
        if (model.getSize() > 0)
            model.clear();
        for (File f : files)
            model.addElement(f);
    }

    public void addFile(File f) {
        model.addElement(f);
    }

    // N.B. getSelectedIndex() returns -1 when no selection
    public void insertAfterSelection(File f) {
        int selected = fileListView.getSelectedIndex() + 1;
        if (selected < model.getSize())
            model.add(selected, f);
        else
            model.addElement(f);
    }

    public void removeSelection() {
        int selected = fileListView.getSelectedIndex();
        if (selected < 0)
            return; // nothing selected
        model.remove(selected);
    }
    
    public void clearFiles() {
        model.clear();
    }

    public Vector<File> getFiles() {
        int n = model.getSize();
        Vector<File> result = new Vector<File>(n);
        for (int i = 0; i < n; ++i)
            result.add(model.getElementAt(i));
        return result;
    }

    public boolean isEmpty() {
        return model.isEmpty();
    }

    public int getFileCount() {
        return model.getSize();
    }

    public File getFileAt(int index) {
        return (File) model.get(index);
    }

    public File getLast() {
        if (model.isEmpty())
            return null;
        return (File) model.get(model.getSize() - 1);
    }

    public File getSelection() {
        return fileListView.getSelectedValue();
    }

    public int getSelectionIndex() {
        return fileListView.getSelectedIndex();
    }

    public void setSelection(File f) {
        fileListView.setSelectedValue(f, true);
    }

    private class CellRenderer
        extends JLabel
        implements ListCellRenderer<File> {

        public CellRenderer() {
            setOpaque(true);
            // fonts?
        }

        @Override
        public Component getListCellRendererComponent(JList<? extends File> list,
                                                      File value,
                                                      int index,
                                                      boolean isSelected,
                                                      boolean cellHasFocus) {
            if (value != null)
                setText(value.getAbsolutePath());
            else
                setText(value.toString());

            // determine appearance
            JList.DropLocation dropLocation = list.getDropLocation();
            if (dropLocation != null
                && !dropLocation.isInsert()
                && dropLocation.getIndex() == index) {
                // cell represents the current DnD drop location
                setBackground(YELLOW);
                setForeground(BLACK);
            } else if (isSelected) {
                // cell is selected
                setBackground(PINK);
                setForeground(BLACK);
            } else {
                // unselected, and not the DnD drop location
                setBackground(WHITE);
                setForeground(BLACK);
            };

            return this;
        }
    }

    //
    // Drag & drop support
    // 
    
    private class FileListTransferHandler
        extends TransferHandler {

        private int selectedIdx = -1;

        public FileListTransferHandler() {
            super();
        }

        public int getSourceActions(JComponent c) {
            return COPY_OR_MOVE;
        }

        protected Transferable createTransferable(JComponent c) {
            JList list = (JList)c;
            selectedIdx = list.getSelectedIndex(); // hidden state, saved between calls
            return new TransferableFile((File) list.getSelectedValue());
        }
        
        public boolean canImport(TransferHandler.TransferSupport info) {
            // Check for String and File flavors
            if (!info.isDataFlavorSupported(fileDataFlavor) // preferred
                && !info.isDataFlavorSupported(DataFlavor.stringFlavor))
                return false;

            // Check supported actions
            int actions = info.getSourceDropActions();
            if ((COPY & actions) == COPY) {
                info.setDropAction(COPY);
                return true;
            }
            else if ((MOVE & actions) == MOVE) {
                info.setDropAction(MOVE);
                return true;
            }

            return false;
        }

        @SuppressWarnings("unchecked")
        public boolean importData(TransferHandler.TransferSupport info) {
            if (!info.isDrop()) {
                return false;
            }

            // Get the object being dropped and turn it into a File.
            Transferable t = info.getTransferable();
            File data;
            if (t.isDataFlavorSupported(fileDataFlavor))
                try {
                    data = (File) t.getTransferData(fileDataFlavor);
                }
                catch (Exception e) {
                    return false;
                }
            else if (t.isDataFlavorSupported(DataFlavor.stringFlavor))
                try {
                    data = new File((String) t.getTransferData(DataFlavor.stringFlavor));
                } 
                catch (Exception e) {
                    return false;
                }
            else
                return false;

            JList list = (JList)info.getComponent();
            DefaultListModel<File> listModel = (DefaultListModel<File>) list.getModel(); // unchecked cast
            JList.DropLocation dl = (JList.DropLocation)info.getDropLocation();
            int index = dl.getIndex();
                                
            if (dl.isInsert())
                listModel.add(index, data);
            else if (index < listModel.getSize())
                listModel.set(index, data); // replacing existing item
            else
                listModel.add(index, data); // adding at end
            return true;
        }

        protected void exportDone(JComponent c, Transferable t, int action) {
            JList source = (JList) c;
            DefaultListModel listModel = (DefaultListModel)source.getModel();
            if (action == TransferHandler.MOVE)
                listModel.remove(selectedIdx);
        
            selectedIdx = -1;
        }
    }

    private class TransferableFile
        implements Transferable {

        File theFile;

        public TransferableFile(File f) {
            theFile = f;
        }

        public Object getTransferData(DataFlavor flavor)
            throws UnsupportedFlavorException,
                   IOException {
            if (theFile == null)
                return null;
            if (flavor.getRepresentationClass() == fileDataFlavor.getRepresentationClass())
                return theFile;
            if (flavor == DataFlavor.stringFlavor)
                return theFile.getAbsolutePath();
            throw new UnsupportedFlavorException(flavor);
        }

        // TODO: make result array static somehow
        public DataFlavor[] getTransferDataFlavors() {
            DataFlavor[] result = {fileDataFlavor,
                                   DataFlavor.stringFlavor};
            return result;
        }

        public boolean isDataFlavorSupported(DataFlavor flavor) {
            return (flavor.getRepresentationClass() == fileDataFlavor.getRepresentationClass()
                    || flavor == DataFlavor.stringFlavor);
        }
    }
    
}
