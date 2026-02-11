## Implementare Text Editor

### Prezentarea generala a structurii

* Pentru Text Editor-ul pe care l-am avut de implementat am folosit o structura de date numita `Piece Table`. Am ales aceasta structura pentru a amortiza 
costul operatiilor de adaugare si introducere de text.

```
typedef struct Buffers {
  char *originalBuffer;
  char *addBuffer;
} Buffers;
```

* Am ales sa fac o structura separata pentru cele doua buffere in care se poate scrie, respectiv `originalBuffer` si `addBuffer`. In `originalBuffer` se pune
textul deja existent in Text Editor atunci cand el este deschis, iar in `addBuffer` se pune textul pe care il scrie utilizatorul.

```
typedef struct pieceTableNode {
  int bufferType;
  int indexOfBuffer;
  int bufferLength; 
  struct pieceTableNode *nextColumn;
} pieceTableNode;
```

* Fiecare modificare se retine cu ajutorul unui nod dintr-o lista inlantuita, respectiv `pieceTableNode`. Structura contine tipul buffer-ului in care
se face scrierea (`bufferType`), indexul la care se face aceasta scriere (`indexOfBuffer`), lungimea textului si urmatorul nod din lista (`nextColumn`). Basically,
aceasta lista permite implementarea operatiilor de `add_text` si `delete_text` fara sa se realizeze o **stergere propriu-zisa** a caracterelor din buffere, ci doar sa se stie cand s-a realizat o adaugare sau o stergere.

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

* Structura de text editor contine structura cu cele doua buffere in care se fac scrierile (`Buffers`), pozitia de scriere la care se adauga de catre utilizator textul
(`indexOfAddBuffer`), dimensiunea acestuia (`addBufferLength`), pozitia globala a cursorului in Text Editor (`cursorPosition`), numarul total de caractere din buffer (`totalLength`), prima stare din lista simplu inlantuita
(`firstColumn`) si numarul total de astfel de stari din aceasta (`totalNumberOfColumns`).

### Complexitatile functiilor implementate

```
TextEditor *create_table(const char *filename, const char *text, int global_cursor)
```
* Functia aloca memoria pentru un nou Text Editor si initializeaza cele doua buffere. In functie e input datele sunt extrase dintr-un fisier sau dintr-un sir de caractere. Se creeaza si primul nod al listei.
* **Complexitatea** acestei operatii este de **O(n)** deoarece trebuie parcurs textul din fisier sau sirul de caractere pentru a fi inserat in `originalBuffer`

```
void advance_cursor(TextEditor *editor, int advance)
```
* Functia muta cursorul cu un numar de pozitii primite in parametrul `advance` fie la _dreapta_ (_advance_ > 0), fie la _stanga_ (_advance_ < 0). 
* **Complexitatea** operatiei de mutare a cursorului este **O(1)**, deoarece doar se acceseaza diferite variabile deci are o complexitate constanta.

```
int show_global_cursor(TextEditor *editor)
```
* Functia arata pozitia cursorului in Text Editor
* **Complexitatea** acestei functii este una constanta si egala cu **O(1)** deoarece trebuie doar accesata variabila `cursorPosition` din structura Text Editor care 
contine pozitia globala a cursorului

```
int show_total_len(TextEditor *editor)
```
* Functia returneaza numarul total de caractere tinut in structura `Piece Table`.
* **Complexitatea** este una constanta data de accesarea variabilei din structura, respectiv **O(1)**.

```
void add_text(TextEditor *editor, char *text)
```
* Functia de inserare a textului adauga un nou sir de caractere in `addBuffer` a carui capacitate o dubleaza in cazul in care lungimea stringului este mai mare
decat numarul de caractere care mai pot fi inserate in array. Se gaseste pozitia nodului unde trebuie facuta inserarea in lista si apoi se trateaza cazul
respectiv de inserare. Daca inserarea se face in interiorul unui nod atunci se face split acestuia.
* **Complexitatea** acestei functii depinde de numarul de noduri exitente in lista de stari si de lungimea textului adaugat in `addBuffer`. Aceasta este liniara, fiind de forma _O(nr noduri + lungimea textului)_, deci o putem considera **O(n)**.

```
void delete_text(TextEditor *editor, int length)
```
* Functia realizeaza _"stergerea"_ unui sir de caractere din editorul de text, aflat cu `length` caractere inaintea cursorului. Operatia se realizeaza prin retinerea 
offset-urilor la care se afla sirul de caractere de cursor. Dupa ce se afla cele doua offset-uri se face stergerea nodului/nodurilor care contine starea de inserare a 
textului respectiv in lista. La final se modifica pozitia cursorului.
* **Complexitatea** functiei de `delete_text` este una liniara, respectiv **O(n)** unde n reprezinta numarul de noduri din lista care trebuie parcurse pana la gasirea 
starii in care au fost inserate caracterele care trebuie sterse.

```
char *extract_current_text(TextEditor *editor)
```
* Functia realizeaza extragerea textului curent din Text Editor. Se parcurg toate nodurile din lista, apoi se verifica tipul buffer-ului, iar in functia de acesta 
copiaza utilizand functia `memcpy` pentru a copia continutul acestora intr-un sir de caractere.
* **Complexitatea** functiei depinde de numarul total de caractere din text, dar si numarul de noduri existent in lista de stari. Complexitatea este de tip 
_O(nr noduri + nr caractere)_, deci **O(n)**.

```
void save_current_text(TextEditor *editor, const char *filename, char **text, int *global_cursor)
```
* Functia utilizeaza intern `extract_current_text` pentru a extrage textul din Text Editor sub forma unui sir de caractere. Dupa apelul functiei, sirul de caractere 
extras se scrie intr-un fisier de iesire si ramane salvat in text, iar cursorul este actualizat. 
* **Complexitatea** acesteia este data de apelul catre `extract_current_text`. Din acest motiv complexitatea acestei functii coincide cu cea liniara a functiei 
anterioare, respectiv **O(n)**.

### Functii auxiliare

```
pieceTableNode *allocateNode(int bufferType, int bufferLength, int indexOfBuffer, pieceTableNode *nextColumn)
```
* Functia aloca un nou nod in lista de stari continuta de structura `Piece Table`, mai exact este o alocare a unui nod dintr-o _lista simplu inlantuita_.

```
int addTextInBuffer (TextEditor *editor, char *text, int len)
```
* Functia adauga un sir de caractere intr-unul dintre cele doua buffere folosind `memcpy`.

```
pieceTableNode *splitNode (TextEditor* editor, pieceTableNode *node, int relativeOffset)
```
* Functia imparte un nod in doua altele atunci cand inserarea din functia `add_text` se face in interior.

```
void findNodePositionInLinkedList(TextEditor *editor, int searchedPosition, pieceTableNode **currentColumn, pieceTableNode **previousColumn, int *relativeOffset)
```
* Functia cauta pozitia unui nod in lista.

```
char *extractFileTextFromBuffer(char *filename, int *sizeOfTheFile)
```
* Functia se extrage textul dintr-un fisier si se pune intr-un buffer temporar.

```
void freeTextEditor (TextEditor *editor)
```
* Functia elibereaza structura `TextEditor`.

```
void insertNodeInLinkedList(TextEditor *editor, pieceTableNode *newNode, pieceTableNode *currentColumn, pieceTableNode *previousColumn, int relativeOffset)
```
* Functia face inserarea unui nod in lista de stari, mai exat inserarea unui nod intr-o lista simplu inlantuita.

```
void removeNodeFromLinkedList(TextEditor *editor, pieceTableNode *startPositionPtr, pieceTableNode *endPositionPtr, pieceTableNode *previousStartPositionPtr, int offsetForStart, int offsetForEnd)
```
* Functia face extragerea unui nod dintr-o lista de stari, indiferent de situatia in care se afla.