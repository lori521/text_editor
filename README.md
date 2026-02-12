## Text Editor Implementation

### General Structure Overview

* For the Text Editor implementation, I utilized a data structure called `Piece Table`. I chose this structure to amortize the cost of add and insert operations.

```
typedef struct Buffers {
  char *originalBuffer;
  char *addBuffer;
} Buffers;
```

* I chose to create a separate structure for the two buffers where writing can occur: `originalBuffer` and `addBuffer`. The `originalBuffer` stores the text that already exists in the Text Editor when opened, while `addBuffer` stores the text written by the user.

```
typedef struct pieceTableNode {
  int bufferType;
  int indexOfBuffer;
  int bufferLength; 
  struct pieceTableNode *nextColumn;
} pieceTableNode;
```

* Each modification is retained using a node in a linked list, specifically a `pieceTableNode`. The structure contains the type of buffer where the writing occurs (`bufferType`), the index where this writing takes place (`indexOfBuffer`), the text length and the next node in the list (`nextColumn`). Basically, this list allows the implementation of `add_text` and `delete_text` operations without performing an **actual deletion** of characters from the buffers, but rather by tracking when an _addition_ or _deletion_ has occurred.

```
typedef struct TTextEditor {
  struct Buffers *usedBuffers;
  
  int indexOfAddBuffer;
  int addBufferLength;
  int cursorPosition;
  int totalLength;

  struct pieceTableNode *firstColumn;
  int totalNumberOfColumns;
} TextEditor;
```
* The text editor structure contains the two write buffers structure (`Buffers`), the write position where the user text is added (`indexOfAddBuffer`), its size (`addBufferLength`), the global cursor position in the Text Editor (`cursorPosition`), the total number of characters in the buffer (`totalLength`), the first state in the singly linked list (`firstColumn`), and the total number of such states within it (`totalNumberOfColumns`).

### Complexities of Implemented Functions

```
TextEditor *create_table(const char *filename, const char *text, int global_cursor)
```

* The function allocates memory for a new Text Editor and initializes the two buffers. Depending on the input, data is extracted from a file or a character string. The first node of the list is also created.
* The **complexity** of this operation is **O(n)** because the text from the file or string must be traversed to be inserted into `originalBuffer`.

```
void advance_cursor(TextEditor *editor, int advance)
```

* The function moves the cursor by a number of positions received in the `advance` parameter, either to the _right_ (_advance_ > 0) or to the _left_ (_advance_ < 0).

The **complexity** of the cursor movement operation is **O(1)**, as it only accesses variables, resulting in constant complexity.

```
int show_global_cursor(TextEditor *editor)
```
* The function shows the cursor position in the Text Editor.
* The **complexity** of this function is constant, **O(1)**, as it only requires accessing the `cursorPosition` variable from the Text Editor structure which holds the global cursor position.

```
int show_total_len(TextEditor *editor)
```
* The function returns the total number of characters held in the `Piece Table` structure.
The **complexity** is _constant_, determined by accessing the variable from the structure, **O(1)**.

```
void add_text(TextEditor *editor, char *text)
```
* The text insertion function adds a new string to `addBuffer`, doubling its capacity if the string length is greater than the number of characters that can fit in the array. The position of the node where insertion must occur is found, and the respective insertion case is handled. If insertion happens inside a node, then the node is split.
* The **complexity** of this function depends on the number of existing nodes in the state list and the length of the text added to `addBuffer`. This is linear, in the form of _O(no. of nodes + text length)_, so we can consider it **O(n)**.

```
void delete_text(TextEditor *editor, int length)
```
* The function performs the _"deletion"_ of a character string from the text editor, located `length` characters before the cursor. The operation is performed by tracking the offsets where the character string is located relative to the cursor. Once the two offsets are found, the node(s) containing the insertion state of that text are deleted from the list. Finally, the cursor position is updated.
* The **complexity** of the `delete_text` function is linear, **O(n)**, where n represents the number of nodes in the list that must be traversed to find the state where the characters to be deleted were inserted.

```
char *extract_current_text(TextEditor *editor)
```
* The function performs the extraction of the current text from the Text Editor. All nodes in the list are traversed, the buffer type is checked, and based on that, the content is copied using `memcpy` into a character string.
* The **complexity** of the function depends on the total number of characters in the text, as well as the number of existing nodes in the state list. The complexity is of the type _O(no. of nodes + no. of characters)_, thus **O(n)**.

```
void save_current_text(TextEditor *editor, const char *filename, char **text, int *global_cursor)
```
* The function internally uses `extract_current_text` to extract the text from the Text Editor as a character string. After the function call, the extracted string is written to an output file and remains saved in text, and the cursor is updated.
* Its **complexity** is determined by the call to `extract_current_text`. For this reason, the complexity of this function coincides with the linear complexity of the previous function, namely **O(n)**.

### Auxiliary Functions

```
pieceTableNode *allocateNode(int bufferType, int bufferLength, int indexOfBuffer, pieceTableNode *nextColumn)
```
* The function allocates a new node in the state list contained by the `Piece Table` structure; specifically, it allocates a node for a _singly linked list_.

```
int addTextInBuffer (TextEditor *editor, char *text, int len)
```
* The function adds a character string into one of the two buffers using `memcpy`.

```
pieceTableNode *splitNode (TextEditor* editor, pieceTableNode *node, int relativeOffset)
```
* The function splits a node into two others when the insertion in `add_text` occurs internally (in the middle of a node).

```
void findNodePositionInLinkedList(TextEditor *editor, int searchedPosition, pieceTableNode **currentColumn, pieceTableNode **previousColumn, int *relativeOffset)
```
* The function searches for the position of a node in the list.

```
char *extractFileTextFromBuffer(char *filename, int *sizeOfTheFile)
```
* The function extracts text from a file and places it into a temporary buffer.

```
void freeTextEditor (TextEditor *editor)
```
* The function frees the `TextEditor` structure.

```
void insertNodeInLinkedList(TextEditor *editor, pieceTableNode *newNode, pieceTableNode *currentColumn, pieceTableNode *previousColumn, int relativeOffset)
```
* The function inserts a node into the state list, specifically inserting a node into a singly linked list.

```
void removeNodeFromLinkedList(TextEditor *editor, pieceTableNode *startPositionPtr, pieceTableNode *endPositionPtr, pieceTableNode *previousStartPositionPtr, int offsetForStart, int offsetForEnd)
```
* The function extracts (removes) a node from a state list, regardless of the situation it is in.