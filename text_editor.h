#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

// TextEditor structure - TODO: define your own fields here
typedef struct TTextEditor {
  // TODO: define any fields you need for your implementation
  struct Buffers *usedBuffers;
  
  int indexOfAddBuffer;
  int addBufferLength;
  int cursorPosition;
  int totalLength;

  struct pieceTableNode *firstColumn;
  int totalNumberOfColumns;
} TextEditor;

typedef struct pieceTableNode {
  int bufferType;
  int indexOfBuffer;
  int bufferLength; 
  struct pieceTableNode *nextColumn;
} pieceTableNode;

typedef struct Buffers {
  char *originalBuffer;
  char *addBuffer;
} Buffers;

// ============================================================================
// API Functions - You MUST implement all of these
// ============================================================================

// Create a new text editor
// filename: if not NULL, load content from this file
// text: if not NULL (and filename is NULL), initialize with this text
// global_cursor: initial cursor position
TextEditor *create_table(const char *filename, const char *text, int global_cursor);

// Move cursor by 'advance' positions (can be negative for backwards)
void advance_cursor(TextEditor *editor, int advance);

// Get current cursor position
int show_global_cursor(TextEditor *editor);

// Get total text length
int show_total_len(TextEditor *editor);

// Insert text at current cursor position
void add_text(TextEditor *editor, char *text);

// Delete 'length' characters before cursor
void delete_text(TextEditor *editor, int length);

// Extract full text as allocated string (caller must free)
char *extract_current_text(TextEditor *editor);

// Save text to file or return via text pointer
void save_current_text(TextEditor *editor, const char *filename, char **text, int *global_cursor);

// ============================================================================
// Helper functions (optional - you can define your own)
// ============================================================================

// aloca un nou nod pt piece table
pieceTableNode *allocateNode(int bufferType, int bufferLength, int indexOfBuffer, pieceTableNode *nextColumn);

// adaugarea unui text in addBuffer
int addTextInBuffer (TextEditor *editor, char *text, int len);

// returneaza partea care se rupe din nod
pieceTableNode *splitNode (TextEditor* editor, pieceTableNode *node, int relativeOffset);

// citire din fisier si returnarea unui buffer care sa contina textul
char *extractFileTextFromBuffer(char *filename, int *sizeOfTheFile);

// eliberare a editorului de text
void freeTextEditor (TextEditor *editor);

// inserare in lista de stari
void insertNodeInLinkedList(TextEditor *editor, pieceTableNode *newNode, pieceTableNode *curr, pieceTableNode *prev, int relativeOffset);

// stergere din lista de stari
void removeNodeFromLinkedList(TextEditor *editor, pieceTableNode *startPositionPtr, pieceTableNode *endPositionPtr, pieceTableNode *previousStartPositionPtr, int offsetForStart, int offsetForEnd);

// se gaseste pozitia relativa a unui nod
void findNodePositionInLinkedList(TextEditor *editor, int searchedPosition, pieceTableNode **currentColumn, pieceTableNode **previousColumn, int *relativeOffset);

#endif // TEXT_EDITOR_H
