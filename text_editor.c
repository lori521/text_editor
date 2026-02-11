#include "text_editor.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

TextEditor *create_table(const char *filename, const char *text,
                         int global_cursor) {
  // TODO: Implement this function
  TextEditor *newTextEditor = (TextEditor *)malloc(sizeof(TextEditor));
  if (!newTextEditor) {
    fprintf(stderr, "the text editor could not be allocated\n");
    return NULL;
  }
  newTextEditor->usedBuffers = (Buffers *)malloc(sizeof(Buffers));
  if (!newTextEditor->usedBuffers) {
    freeTextEditor(newTextEditor);
    fprintf(stderr, "the buffers for the text editor could not be allocated\n");
    return NULL;
  }

  int initialBufferLength = 1024;
  newTextEditor->usedBuffers->addBuffer = (char *)malloc(initialBufferLength * sizeof(char));
  if (!newTextEditor->usedBuffers->addBuffer) {
    freeTextEditor(newTextEditor);
    fprintf(stderr, "the add buffer for the text editor could not be allocated\n");
    return NULL;
  }
  newTextEditor->addBufferLength = initialBufferLength;
  newTextEditor->indexOfAddBuffer = 0;
  
  char *tempBufferForFileText = NULL;
  long int capacityOfTempBuffer;
  if (filename != NULL) {

    tempBufferForFileText = extractFileTextFromBuffer(filename, &capacityOfTempBuffer);

    if(tempBufferForFileText == NULL) {
      freeTextEditor(newTextEditor);
      fprintf(stderr, "the file could not be opened\n");
      return NULL;
    }

  } else if (text != NULL) {
    tempBufferForFileText = strdup(text);
    capacityOfTempBuffer = strlen(text);
  } else {
    tempBufferForFileText = strdup("");
    capacityOfTempBuffer = 0;
  }

  newTextEditor->usedBuffers->originalBuffer = tempBufferForFileText;

  pieceTableNode *firstPieceTableNode = allocateNode(0, capacityOfTempBuffer, 0, NULL);
  if (!firstPieceTableNode) {
    freeTextEditor(newTextEditor);
    fprintf(stderr, "the text from the file could not be allocated\n");
    return NULL;
  }

  newTextEditor->firstColumn = firstPieceTableNode;
  newTextEditor->totalNumberOfColumns = 1;
  newTextEditor->cursorPosition = global_cursor;
  newTextEditor->totalLength = capacityOfTempBuffer;
  return newTextEditor;
}

void advance_cursor(TextEditor *editor, int advance) {
  // TODO: Implement this function
  pieceTableNode *ptr = editor->firstColumn;

  int supposedPosition = editor->cursorPosition + advance;

  if(supposedPosition > editor->totalLength) {
    editor->cursorPosition = editor->totalLength;
  } else if (supposedPosition < 0) {
    editor->cursorPosition = 0;
  } else {
    editor->cursorPosition = supposedPosition;
  }
}

int show_global_cursor(TextEditor *editor) {
  // TODO: Implement this function
  if (editor == NULL)
    return 0;
  
  return editor->cursorPosition;
}

int show_total_len(TextEditor *editor) {
  // TODO: Implement this function
  if (editor == NULL)
    return 0;
  return editor->totalLength;
}

void add_text(TextEditor *editor, char *text) {
  // TODO: Implement this function
  int bufferUpdatedLength = strlen(text);

  pieceTableNode *currentColumn = NULL;
  pieceTableNode *previousColumn = NULL;

  pieceTableNode *newTextAddition = allocateNode(1, bufferUpdatedLength, editor->indexOfAddBuffer, NULL);
  if (!newTextAddition) {
    fprintf(stderr, "could not allocate new text addition\n");
    return;
  }

  addTextInBuffer(editor, text, strlen(text));

  // aici chestie cu pozitie
  int relativeOffset = 0;
  int searchedPosition = editor->cursorPosition;

  findNodePositionInLinkedList(editor, searchedPosition, &currentColumn, &previousColumn, &relativeOffset);

  insertNodeInLinkedList(editor, newTextAddition, currentColumn, previousColumn, relativeOffset);

  editor->totalNumberOfColumns++;
  editor->cursorPosition += bufferUpdatedLength;
  editor->totalLength += bufferUpdatedLength;
}

void delete_text(TextEditor *editor, int length) {
  // TODO: Implement this function
  if (length <= 0)
    return;
  
  int startPosition = editor->cursorPosition - length;
  int endPositionn = editor->cursorPosition;

  if (startPosition < 0) 
    startPosition = 0;

  // find where positions are
  pieceTableNode *currentColumn = editor->firstColumn;
  pieceTableNode *previousColumn = NULL;
  pieceTableNode *startPositionPtr = NULL;
  pieceTableNode *endPositionPtr = NULL;
  pieceTableNode *previousStartPositionPtr;
  pieceTableNode *previousEndPositionPtr;
  int offsetForStart = 0;
  int offsetForEnd = 0;

  findNodePositionInLinkedList(editor, startPosition, &startPositionPtr, &previousStartPositionPtr, &offsetForStart);

  findNodePositionInLinkedList(editor, endPositionn, &endPositionPtr, &previousEndPositionPtr, &offsetForEnd);

  if (startPositionPtr == NULL) {
    return; 
  }

  // apel pt eliminare din lista 
  removeNodeFromLinkedList(editor, startPositionPtr, endPositionPtr, previousStartPositionPtr, offsetForStart, offsetForEnd);
  editor->addBufferLength -= length;
  editor->totalLength -= length;
  editor->cursorPosition -= length;
}

char *extract_current_text(TextEditor *editor) {
  // TODO: Implement this function
  if (editor == NULL) 
    return NULL;
  
  int totalTextEditorCharacters = show_total_len(editor);
  char *textFromTextEditor = (char *)malloc((totalTextEditorCharacters + 1) * sizeof(char));
  if (textFromTextEditor == NULL) {
    fprintf(stderr, "could not extract text because the return buffer could not be allocated\n");
    return NULL;
  }

  pieceTableNode *ptr = editor->firstColumn;
  int writePosition = 0;

  while (ptr != NULL) {
     // check buffer type
    char  *currentBuffer;
    if (ptr->bufferType == 0)
      currentBuffer = editor->usedBuffers->originalBuffer;
    else
      currentBuffer = editor->usedBuffers->addBuffer;
    // copy length between parameters 
    memcpy(textFromTextEditor + writePosition, currentBuffer + ptr->indexOfBuffer, ptr->bufferLength);
    
    writePosition += ptr->bufferLength;

    ptr = ptr->nextColumn;
  }
 
  textFromTextEditor[totalTextEditorCharacters] = '\0';
  return textFromTextEditor;
}

void save_current_text(TextEditor *editor, const char *filename, 
                       char **text, int *global_cursor) {
  // TODO: Implement this function
  if (editor == NULL)
    return;

  char *currentTextInEditor = extract_current_text(editor);

  if (filename != NULL) {
    FILE *writeFile = fopen(filename, "w");
    if (writeFile == NULL) {
      fprintf(stderr, "could not open write file\n");
    } else {
      fputs(currentTextInEditor, writeFile);
      fclose(writeFile);
    }
  }

  if (text != NULL)
    *text = currentTextInEditor;
  else 
    free(currentTextInEditor);

  if(global_cursor != NULL)
    *global_cursor = editor->cursorPosition;
}

/* functii auxiliare pentru modularizarea codului */

// alocarea unui nou nod pt piece table
pieceTableNode *allocateNode(int bufferType, int bufferLength, int indexOfBuffer, pieceTableNode *nextColumn) {
  pieceTableNode *newNode = (pieceTableNode *)malloc(1 * sizeof(pieceTableNode));
  if (!newNode)
    return NULL;

  newNode->bufferType = bufferType;
  newNode->bufferLength = bufferLength;
  newNode->indexOfBuffer = indexOfBuffer;
  newNode->nextColumn = nextColumn;

  return newNode;
}

// adaugarea unui text in addBuffer
int addTextInBuffer (TextEditor *editor, char *text, int len) {
  while (editor->indexOfAddBuffer +len >= editor->addBufferLength) {
    // realloc
    editor->addBufferLength *= 2;
    char *tempBuffer = realloc(editor->usedBuffers->addBuffer, editor->addBufferLength);
    if (!tempBuffer) {
      fprintf(stderr, "could not double buffer\n");
      return -1;
    }
    editor->usedBuffers->addBuffer = tempBuffer;
  }

  // then copy
  int startIndex = editor->indexOfAddBuffer;
  memcpy(editor->usedBuffers->addBuffer + editor->indexOfAddBuffer, text, len);
  editor->indexOfAddBuffer += len;

  return startIndex;
}

// se face split la nod
pieceTableNode *splitNode (TextEditor* editor, pieceTableNode *node, int relativeOffset) {
  int indexOfAddBuffer = node->indexOfBuffer + relativeOffset;
  int bufferedLength = node->bufferLength - relativeOffset;
  pieceTableNode *afterCursorPart = allocateNode(node->bufferType, bufferedLength, indexOfAddBuffer, node->nextColumn);
  if(!afterCursorPart) {
    fprintf(stderr, "text could not be inserted\n");
    return NULL;
  }

  return afterCursorPart;
}

// se gaseste pozitia relativa a unui nod
void findNodePositionInLinkedList(TextEditor *editor, int searchedPosition, pieceTableNode **currentColumn, pieceTableNode **previousColumn, int *relativeOffset) {
  pieceTableNode *tempCurrentColumn = editor->firstColumn;
  pieceTableNode *tempPreviousColumn = NULL;
  int offset = 0;

  while (tempCurrentColumn != NULL && offset + tempCurrentColumn->bufferLength < searchedPosition) {
    offset += tempCurrentColumn->bufferLength;
    tempPreviousColumn = tempCurrentColumn;
    tempCurrentColumn = tempCurrentColumn->nextColumn;
  }

  *relativeOffset = searchedPosition - offset;

  *currentColumn = tempCurrentColumn;
  *previousColumn = tempPreviousColumn;
}


// citire din fisier si returnarea unui buffer care sa contina textul
char *extractFileTextFromBuffer(char *filename, int *sizeOfTheFile) {
  FILE *file = fopen(filename, "r");
  if(file == NULL)
    return NULL;

  fseek(file, 0L, SEEK_END);
  *sizeOfTheFile = ftell(file);
  fseek(file, 0L, SEEK_SET);

  char *tempBufferForFileText = (char *)malloc((*sizeOfTheFile + 1) * sizeof(char));
  if (!tempBufferForFileText) {
    return NULL;
  }

  fread(tempBufferForFileText, 1, *sizeOfTheFile, file);
  tempBufferForFileText[*sizeOfTheFile] = '\0';

  fclose(file);
  return tempBufferForFileText;
}

// eliberare a editorului de text
void freeTextEditor (TextEditor *editor) {
  if (editor == NULL) 
    return;

  pieceTableNode *ptr = editor->firstColumn;
  pieceTableNode *freePos = NULL;

  while (ptr != NULL) {
    freePos = ptr;
    ptr = ptr->nextColumn;
    free(freePos);
  }

  if (editor->usedBuffers != NULL) {
    if (editor->usedBuffers->addBuffer != NULL)
      free(editor->usedBuffers->addBuffer);
    if (editor->usedBuffers->originalBuffer)
      free(editor->usedBuffers->originalBuffer);
  
    free(editor->usedBuffers);
  }

  free(editor);
}

// inserare in lista de stari
void insertNodeInLinkedList(TextEditor *editor, pieceTableNode *newNode, pieceTableNode *currentColumn, pieceTableNode *previousColumn, int relativeOffset) {
  pieceTableNode *afterCursorPart = NULL;

  if (currentColumn == NULL) {
    if (previousColumn == NULL)
      editor->firstColumn = newNode;
    else 
      previousColumn->nextColumn = newNode;
  } else if (relativeOffset == 0) {
    // insert between two pieceTableNodes
    newNode->nextColumn = currentColumn;
    if (previousColumn == NULL)
      editor->firstColumn = newNode;
    else 
      previousColumn->nextColumn = newNode;
  } else {
    // aici apel la splitNode
    afterCursorPart = splitNode(editor, currentColumn, relativeOffset);

    currentColumn->bufferLength = relativeOffset;

    // link nodes
    currentColumn->nextColumn = newNode;
    newNode->nextColumn = afterCursorPart;

    editor->totalNumberOfColumns++;
  }
  
}

// stergere din lista inlantuita
void removeNodeFromLinkedList(TextEditor *editor, pieceTableNode *startPositionPtr, pieceTableNode *endPositionPtr, pieceTableNode *previousStartPositionPtr, int offsetForStart, int offsetForEnd) {
  // case one for delete
  if (startPositionPtr == endPositionPtr) {
    pieceTableNode *nextNode = startPositionPtr->nextColumn; 

    pieceTableNode *afterDeletePart = NULL;

    if (startPositionPtr->bufferLength - offsetForEnd > 0) {
      int indexOfBuffer = endPositionPtr->indexOfBuffer + offsetForEnd;
      int bufferLength = endPositionPtr->bufferLength - offsetForEnd;

      afterDeletePart = allocateNode(startPositionPtr->bufferType, bufferLength, indexOfBuffer, nextNode);
      if(!afterDeletePart) {
        fprintf(stderr, "text could not be deleted\n");
        return;
      }
      editor->totalNumberOfColumns++;
    } else {
      afterDeletePart = nextNode;
    }

    if (offsetForStart > 0) {
      startPositionPtr->bufferLength = offsetForStart;
      startPositionPtr->nextColumn = afterDeletePart;
    } else {
      if (previousStartPositionPtr == NULL)
        editor->firstColumn = afterDeletePart;
      else
        previousStartPositionPtr->nextColumn = afterDeletePart;

      free(startPositionPtr);
      editor->totalNumberOfColumns--;
    }
  } else {
    startPositionPtr->bufferLength = offsetForStart;

    if (endPositionPtr != NULL) {
      endPositionPtr->indexOfBuffer += offsetForEnd;
      endPositionPtr->bufferLength -= offsetForEnd;
    }
   

    pieceTableNode *ptr = startPositionPtr->nextColumn;
    while (ptr != endPositionPtr) {
      pieceTableNode *currentNode = ptr;
      ptr = ptr->nextColumn;
      free(currentNode);
      editor->totalNumberOfColumns--;
    }

    pieceTableNode *linkNode = NULL;
    if(endPositionPtr != NULL && endPositionPtr->bufferLength > 0) {
      linkNode = endPositionPtr;
    } else if (endPositionPtr != NULL) {
      linkNode = endPositionPtr->nextColumn;
      free(endPositionPtr);
      editor->totalNumberOfColumns--;
    } 

    if (offsetForStart > 0) {
      startPositionPtr->bufferLength = offsetForStart;
      startPositionPtr->nextColumn = linkNode;
    } else {
      if(previousStartPositionPtr == NULL)
        editor->firstColumn = linkNode;
      else
        previousStartPositionPtr->nextColumn = linkNode;
      free(startPositionPtr);
      editor->totalNumberOfColumns--;
    }
  }
}