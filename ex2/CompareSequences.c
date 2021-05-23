/**
 * @file CompareSequences.c
 * @author  Bar Aloni <baraloni@mail.huji.ac.il>
 * @version 1.0
 * @date Nov 15, 2018
 *
 * @brief reads supplied sequence file, and for each couple of them
 *        prints the Alignment score, followed by the alignments.
 *
 * @section DESCRIPTION
 * Input  :  <sequence file> <match> <mismatch> <gap>
 *          where match, mismatch and gap are ints, sequence file is a txt file
 *          in linux. (for Windows txt file choose mode DEBUG. error:  will print excessive info)
 * Process: reads supplied sequence file, and keeps the sequences in a (linked)list of <Sequence>s.
 *          for each couple of them calculates and prints the Alignment score. during the process,
 *          the algorithm keeps a table of <Cell>s, which allows it to remember the cell from which
 *          the score calculation came, and therefore allows it to traces back the calculations,
 *          and print the alignments.
 * Output : it prints the alignments score for each couple of sequences, followd by the alignments.
 *          when success- exit with 0, else- prints an informative error massage to the
 *          stderr, and exit with EXIT_FAILURE.
 */
 //-------------Includes---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
//-----------Constants-----------------------------------------------------------------------------
#define HEADER_PREFIX '>'
#define MAX_LINE_LEN 100
#define SPACE_CHAR '-'
//-------------Messages----------------------------------------------------------------------------
#define USAGE_ERR_MSG fprintf(stderr,\
                      "Usage: CompareSequences <sequence file> <match> <mismatch> <gap>\n")
#define SEQ_NUM_ERR_MSG(fileName) fprintf(stderr,\
                                  "Error: Number of Sequences in %s < 2.\n", fileName)
//-------------Macros-----------------------------------------------------------------------------
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define TERNARY_MAX(X, Y, Z) (MAX(MAX(X, Y), Z))

#ifdef DEBUG
#define DEBUG_PRINT 1
#define NEW_LINE "\r\n"
#define NEW_LINE_LEN 2
#else
#define DEBUG_PRINT 0
#define NEW_LINE "\n"
#define NEW_LINE_LEN 1
#endif
//--------------------------------------part 1 Logic---------------------------------------------

/**
 * represents a sequence that has:
 * name: string(up to 99 chars != HEADER_PREFIX).
 * value: string of unknown length .
 * length: the length of the value(>0).
 * next: an address of another Sequence object(or Null). meant for ordering sequences objects.
 */
typedef struct Sequence
{
    char *seqName;
    char *seqVal;
    size_t seqLen;
    struct Sequence* next;
}Sequence;

/**
 * initialize sequence object: assign memory and fill the fields in a trivial way.
 * @return the sequence we've created
 */
Sequence *initializeSeq()
{
    Sequence *seq = (Sequence *)malloc(sizeof(Sequence));
    seq->seqName = NULL;
    seq->seqVal = NULL;
    seq->seqVal = 0;
    seq->next = NULL;
    return seq;
}

/**
 * sets the <name> as the <seq>'s seqName
 * @param seq: Sequence Object
 * @param name: string (without "\n" )
 */
void setSeqName(Sequence *seq, const char *const name)
{
    seq->seqName = (char *)malloc(strlen(name) + 1);
    strcpy(seq->seqName, name);
}

/**
* adds the supplied <len> to the <seq>'s len field.
* @param seq: Sequence Object
* @param len: size_t var
*/
void setSeqLen(Sequence *seq, size_t len)
{
    seq->seqLen += len;
}

/**
 * sets the <seq>'s value to be val & updates it's len field accordingly
 * @param seq: Sequence Object
 * @param val: string (without "\n" or HEADER_PREFIX)
 */
void setSeqVal(Sequence *seq, const char *const val)
{
        size_t idx = 0;
        if (seq->seqVal == NULL)
        {
            seq->seqVal = realloc(seq->seqVal, strlen(val) + 1);
        }
        else
        {
            idx = strlen(seq->seqVal);
        }
        strcpy(seq->seqVal + idx, val);
        setSeqLen(seq, strlen(val));
}

/**
 * sets <seq>'s next field to be <next> (if <seq> isn't null. if is: does nothing)
 * @param seq :
 * @param next : sequences object
 */
void setSeqNext(Sequence *seq, Sequence *next)
{
    if (seq != NULL)
    {
        seq->next = next;
    }
}

/**
 * @param seq: Sequence Object
 * @return the <seq>'s name (address)
*/
char *getSeqName(const Sequence *const seq)
{
    return seq->seqName;
}

/**
* @param seq: Sequence Object
* @return the <seq>'s value (address)
*/
char *getSeqVal(const Sequence *const seq)
{
    return seq->seqVal;
}

/**
 * @param seq: Sequence Object
 * @return the <seq>'s length
 */
size_t getSeqLen(const Sequence *const seq)
{
    return seq->seqLen;
}

/**
 * @param seq: Sequence Object
 * @return the <seq>'s next
 */
Sequence *getSeqNext(Sequence *seq)
{
    return seq->next;
}

/**
 * deletes the <seq>
 * @param seq: Sequence Object
 */
void eraseSequence(Sequence *seq)
{
    char *seqName = getSeqName(seq);
    char *seqVal = getSeqVal(seq);
    free(seqName);
    free(seqVal);
    free(seq);
}


/**
* debugging helper: prints the array of sequences in the form: <name1,val1>...<nameN,valN>
* @param sequences
*/
void printSeqArray(Sequence *head)
{
    Sequence *curr = head;
    while (curr != NULL)
    {
        printf("<%s,%s> ", getSeqName(curr), getSeqVal(curr));
        curr = getSeqNext(curr);
    }
    printf("\n");
}

//---------------Part 1 flow: read seq file & keep data-------------------------------------------
/**
 * @param str: pointer to a valid char[]
 * @return str return <str> without NEW_LINE
 */
char *refineData(char *str)
{
        return strtok(str, NEW_LINE);
}

/**
 * reads the file and parse it: creates Sequence objects out of the sequences mentioned in the
 * file and keeps them in an array, and returns that array's address.
 * (updates <seqNum> to represents the size of the array)
 * @param file: pointer to a valid file
 * @param seqNum: the size of the array
 */
Sequence *parseSeqFile(FILE *file, size_t *const seqNum)
{
    char buff[MAX_LINE_LEN + NEW_LINE_LEN];
    char *line = fgets(buff, MAX_LINE_LEN + NEW_LINE_LEN, file);
    size_t toStart = 1;

    Sequence *head = NULL;
    Sequence *curr = head;
    Sequence *seq = NULL;

    while (line != NULL) // existing line
    {
        // header line: seq name
        if (toStart && buff[0] == HEADER_PREFIX)
        {
            //allocate memory & initialize new sequence:
            seq = initializeSeq();
            if (head == NULL)
            {
                head = seq;
            }
            setSeqNext(curr, seq);
            curr = seq;
            char *seqName = refineData(&buff[1]);
            setSeqName(seq, seqName);

            ++(*seqNum);
            toStart = 0;
            line = fgets(buff, MAX_LINE_LEN + NEW_LINE_LEN, file);
            continue;
        }
            //co-responding seq val line(s):
        else if (strcmp(line, NEW_LINE) != 0)
        {
            char *seqVal = refineData(buff);
            setSeqVal(seq, seqVal);
            toStart = 1;
        }
        //continue reading:
        line = fgets(buff, MAX_LINE_LEN + NEW_LINE_LEN, file);
    }
    if (DEBUG_PRINT) // prints the deq Array
    {
        printSeqArray(head);
    }
    return head;
}

//------------------------Part 2 Logic------------------------------------------------------------
/**
* represents a Cell in of a score table. Each cell has:
* score: int.
* source: pointer to another Cell object(or null). represents the cell from which we drew the
*         data to calculate the score.
* sourceChar: 'L', 'A' or 'D'. represents the location of the sourceCell.
*            ('L': left of this cell, 'A': above this cell or
*             'D': on diagonal -left&above- of this cell)
*/
typedef struct Cell
{
    struct Cell *sourceCell;
    char sourceChar;
    int score;
}Cell;

/**
 * updates the <cell>'s fields according to the supplied data (<sourceCell>, <score>)
 * @param cell : cell obj
 * @param sourceCell : pointer to another cell obj
 * @param sourceChar: char that represents the location of the <sourceCell> : 'L', 'A', or 'D'
 * @param score : int representing the cell's score
 */
void initializeCell(Cell *const cell, Cell *const sourceCell, \
                    const char sourceChar, const int score)
{
    cell->sourceCell = sourceCell;
    cell->sourceChar = sourceChar;
    cell->score = score;
}

/**
* @param cell: Cell object
* @return the score for the alignment of  the cell.
*/
int getScore(Cell *cell)
{
    return cell->score;
}

    /**
* @param cell: Cell object
* @return the sourceChar of the cell.
*/
char getSourceChar(Cell *cell)
{
    return cell->sourceChar;
}

/**
* @param cell: Cell object
* @return the sourceCell of the cell.
*/
Cell *getSource(Cell *cell)
{
    return cell->sourceCell;
}

    /**
     * calculates: Table[i][j] = Table[i-1][j] + gap
     * @param table: a valid table ( a matrix of <Cell> objects, of dimension: <rows> X <cols>)
     * @param row: the row of the cell (i)
     * @param col: the col of the cell {j)
     * @param g: gap parameter
     * @return the calculation's result.
     */
int calByAbove(Cell **table, const size_t row, const size_t col, const int g)
{
    return getScore(&table[row - 1][col]) + g;
}

/**
* calculates: Table[i][j] = Table[i][j-1] + gap
* @param table: a valid table (a matrix of <Cell> objects, of dimension: <rows> X <cols>)
* @param row: the row of the cell (i)
* @param col: the col of the cell {j)
* @param g: gap parameter
* @return the calculation's result.
*/
int calByLeft(Cell **table, const size_t row, const size_t col, const int g)
{
    return getScore(&table[row][col - 1]) + g;
}

/**
* calculates and returns: if <seq1Val>[i] = <seq2Val>[j]: Table[i][j] = Table[i-1][j-1] + match.
*                             else: Table[i][j] = Table[i-1][j-1] + mismatch.
* @param table: a valid table (a matrix of <Cell> objects, of dimension: <rows> X <cols>)
* @param row: the row of the cell (i)
* @param col: the col of the cell {j)
* @param seq1Val: string representing the value field of a Sequence object
* @param seq2Val: string representing the value field of a Sequence object
* @param m: match parameter
* @param s: mismatch parameter
* @return the calculation's result.
*/
int calByDiagonal(Cell **table, const size_t row, const size_t col, const char *const seq1Val, \
                  const char *const seq2Val, const int m, const int s)
{
    if (seq1Val[row - 1] == seq2Val[col - 1])
    {
        return getScore(&table[row - 1][col - 1]) + m;
    }
    return getScore(&table[row - 1][col - 1]) + s;
}

    /**
     * initialize cell in <table>[<row>][<col>], based on other cells attributes.
     * @param table: a valid table (a matrix of <Cell> objects, of dimension: <rows> X <cols>)
     * @param row: the row of the cell
     * @param col: the col of the cell
     * @param seq1Val: string representing the value field of a Sequence object
     * @param seq2Val: string representing the value field of a Sequence object
     * @param m: match parameter
     * @param s : mismatch parameter
     * @param g : gap parameter
     */
void setCell(Cell **table, const size_t row, const size_t col, const char *const seq1Val, \
             const char *const seq2Val, const int m, const int s, const int g)
{
    // calculate cell's score
    int leftScore = calByLeft(table, row, col, g);
    int aboveScore = calByAbove(table, row, col, g);
    int diagonalScore = calByDiagonal(table, row, col, seq1Val, seq2Val, m, s);
    int max = TERNARY_MAX(leftScore, aboveScore, diagonalScore);
    // calculate cell's source:
    Cell *sourceCell = NULL;
    char sourceChar = 0;
    if(max == diagonalScore)
    {
        sourceCell = &table[row - 1][col - 1];
        sourceChar = 'D';
    }
    else if(max == leftScore)
    {
        sourceCell = &table[row][col - 1];
        sourceChar = 'L';
    }
    else if(max == aboveScore)
    {
        sourceCell = &table[row - 1][col];
        sourceChar = 'A';
    }

    //sets cell:
    initializeCell(&table[row][col], sourceCell, sourceChar, max);
}
/**
 * creates a table: a matrix of <Cell> objects, of dimension: <rows> X <cols>.
 * @param rows: num of rows in <table>.
 * @param cols: num of cols in <table>.
 * @return the table.
 */
Cell **createTable(const size_t rows, const size_t cols)
{
    Cell **table;
    table =	(Cell**)malloc(rows * sizeof(Cell*));
    size_t r;
    for	(r = 0;	r < rows ; r++)
    {
        table[r] = (Cell*)malloc(cols * sizeof(Cell));
    }
    return table;
}

/**
 * initializes the table, i.e: the Cells in the first row and column.
 * @param table: the table to initialize.
 * @param rows: num of rows in <table>.
 * @param cols: num of cols in <table>.
 * @param g: the gap parameter.
 */
void initializeTable(Cell **const table, const size_t rows, const size_t cols, const int g)
{
    size_t r, c;
    initializeCell(&table[0][0], NULL, 0, 0);
    for (r = 1; r < rows; ++r)// initialize first col
    {
        initializeCell(&table[r][0], NULL, 0, (int)r * g);
    }
    for (c = 1; c < cols; ++c) // initialize first row
    {
        initializeCell(&table[0][c], NULL, 0, (int)c * g);
    }
}

/**
 * fills the table: updates all cells and their attributes.
 * @param table: a valid table (a matrix of <Cell> objects)
 * @param seq2: pointer to a Sequence Object.
 * @param seq2: pointer to a Sequence Object.
 * @param m: match parameter
 * @param s : mismatch parameter
 * @param g : gap parameter
 */
void fillTable(Cell **const table, const Sequence *const seq1, const Sequence *const seq2,\
               const int m, const int s, const int g)
{
    const size_t rows = getSeqLen(seq1) + 1;
    const size_t cols = getSeqLen(seq2) + 1;
    const char *const seq1Val = getSeqVal(seq1);
    const char *const seq2Val = getSeqVal(seq2);

    initializeTable(table, rows, cols, g);
    size_t r, c;
    for (r = 1; r < rows; ++r)
    {
        for (c = 1 ; c < cols; ++c)
        {
            setCell(table, r, c, seq1Val, seq2Val, m, s, g);
            if(DEBUG_PRINT) //prints table
            {
                printf("%d(%c) ", getScore(&table[r][c]), getSourceChar(&table[r][c]));
            }
        }
        if(DEBUG_PRINT) // prints table
        {
            printf("\n");
        }
    }
}


/**
* frees the memory allocated to <table>
* @param table: the table to initialize.
* @param rows: num of rows in <table>.
*/
void eraseTable(Cell **table, const size_t rows)
{
    size_t r;
    for(r =	0; r < rows; ++r)
    {
        free(table[r]);
        table[r] = NULL;
    }
    free(table);
}

//-----------------------Part 2 flow: sequences compare-------------------------------------------

/**
 * prints the alignment score for the sequences called <seq1Name>, <seq2Name>,
 * in a specific syntax (see implementation).
 * @param seq1Name: sequence object's name (address).
 * @param seq2Name: sequence object's name (address).
 * @param score: the alignment score for the sequences called <seq1Name>, <seq2Name>.
 */
void printScore(const char *const seq1Name, const char *const seq2Name, const int score)
{
    printf ("Score for alignment of %s to %s is %d\n", seq1Name, seq2Name, score);
}

void analyzeSourceChar(const char sourceChar, char *const seq1Final, char *const seq2Final,\
                       const char *const seq1Val, const char *const seq2Val, size_t *i, size_t *j)
{
    if (sourceChar == 'D')
    {
        seq1Final[0] = seq1Val[*i];
        seq2Final[0] = seq2Val[*j];
        seq1Final[1] = seq2Final[1] = '\0';
        --*i, --*j;
    }
    else if (sourceChar == 'L')
    {
        seq1Final[0] = SPACE_CHAR;
        seq2Final[0] = seq2Val[*j];
        seq1Final[1] = seq2Final[1] = '\0';
        --*j;
    }
    else if (sourceChar == 'A')
    {
        seq1Final[0] = seq1Val[*i];
        seq2Final[0] = SPACE_CHAR;
        seq1Final[1] = seq2Final[1] = '\0';
        --*i;
    }
}

/**
 * create a alignment from the 2 inputs (the alignment params), and frees the memory that held them
 * @param paramLen: the length of the 2 parameters
 * @param seq1Aligned : the aligned param
 * @param seq2Aligned: the aligned param
 * @return the alignment (string of the form <seq1Aligned>\n<seq2Alignment>\n)
 */
char *createAlignment(const size_t paramLen, char *const seq1Aligned, char *const seq2Aligned)
{
    char *alignment = (char *)malloc(sizeof(char) * (2 * paramLen + 1));
    strcpy(alignment, seq1Aligned);
    strcat(alignment, "\n");
    strcat(alignment, seq2Aligned);
    strcat(alignment, "\n");
    free(seq1Aligned);
    free(seq2Aligned);
    return alignment;
}
   /**
    * calculate the alignment for the sequences in a specific syntax (see implementation).
    * @param seq1 : address of a sequence object.
    * @param seq2 : address of a sequence object.
    * @param table: a valid table (a matrix of <Cell> objects, of dimension: <rows> X <cols>)
    * @param rows: num of rows in <table>.
    * @param cols: num of cols in <table>.
    * @return the alignment.
    */
char *calAlignment(const Sequence *const seq1, const Sequence *const seq2,\
                   Cell **table, const size_t rows, const size_t cols)
{
    //initializes vars:
    char *seq1Final = NULL, *seq2Final = NULL ;
    char *seq1Add = "", *seq2Add = "";
    Cell *currCell = &(table[rows - 1][cols - 1] );
    size_t i = getSeqLen(seq1) - 1;
    size_t j = getSeqLen(seq2) - 1;
    const char *const seq1Val = getSeqVal(seq1);
    const char *const seq2Val = getSeqVal(seq2);
    size_t idx = 1;

    while (currCell != NULL)
    {
        char sourceChar = getSourceChar(currCell);
        //set add1, add2 to hold the alignment so far (from end until this point):
        seq1Final = (char *)malloc(sizeof(char) + idx);
        seq2Final = (char *)malloc(sizeof(char) + idx);

        analyzeSourceChar(sourceChar, seq1Final, seq2Final, seq1Val, seq2Val, &i, &j);

        strcat(seq1Final, seq1Add);
        strcat(seq2Final, seq2Add);

        seq1Add = seq1Final;
        seq2Add = seq2Final;
        //move to next iter:
        currCell = getSource(currCell);
        ++idx;
    }
    //creates alignment from the 2 aligned params, frees their memory, and return the new alignment
    return createAlignment(idx, seq1Add, seq2Add);
}

    /**
     * calculates and prints the alignment score for the sequences called <seq1Name>, <seq2Name>,
     * and returns the alignment itself.
     * @param seq1 : address of a sequence object.
     * @param seq2 : address of a sequence object.
     * @param m : match parameter
     * @param s : mismatch parameter
     * @param g : gap parameter
     * @return the alignment.
     */
char *compareSequences(const Sequence *const seq1, const Sequence *const seq2,\
                       const int m, const int s, const int g)
{
    const size_t rows = getSeqLen(seq1) + 1;
    const size_t cols = getSeqLen(seq2) + 1;
    const char *const seq1Name = getSeqName(seq1);
    const char *const seq2Name = getSeqName(seq2);
    Cell **table = createTable(rows, cols);
    fillTable(table, seq1, seq2, m, s, g);
    const int score = getScore(&table[rows - 1][cols - 1]);
    printScore(seq1Name, seq2Name, score);
    char *alignment = calAlignment(seq1, seq2, table, rows, cols);
    eraseTable(table, rows);
    return alignment;
}

//----------------------------Running the program--------------------------------------------------
/**
 * @param str some string
 * @param isInteger
 * @return the converted int that str represents (no change to isInteger).
 * if str doesn't represents int, isInteger will be 0.
 */
long stringToInt(const char *const str, size_t *const isInteger)
{
    char *remaining = NULL;
    long result = strtol(str, &remaining, 10);
    if (strcmp(remaining, "") != 0)
    {
        *isInteger = 0;
    }
    return result;
}

/**
 * erases the supplied Sequence array in address <sequences>.
 * @param sequences : array of type Sequence.
 */
void eraseSequences(Sequence *const sequences)
{
    Sequence *curr = sequences,  *next;
    while (curr != NULL)
    {
        next = getSeqNext(curr);
        eraseSequence(curr);
        curr = next;
    }
}

/**
 * prints & frees the alignments
 * @param alignments : array of size <numOfAli>
 * @param numOfAli: len of <alignments>
 */
void printAlignment(const size_t numOfAli, char **alignments)
{
    size_t idx;
    for (idx = 0; idx < numOfAli; ++idx)
    {
        printf("%s", alignments[idx]);
        free(alignments[idx]);
    }
    free(alignments);
}

/**
 *computes & prints score and alignment for each couple of sequences we've read.
 * @param sequences: pointer to an array of Sequence objects.
 * @param m : match parameter
 * @param s : mismatch parameter
 * @param g : gap parameter
 */
void calAndPrintScores(Sequence *sequences, const int m, const int s, const int g)
{
    // calculate the alignments scores and prints them. keeps the alignments in an array.
    char **alignments = NULL;
    char * alignment = NULL;
    size_t idx = 0;
    Sequence *seq1 = sequences, *seq2;
    while (getSeqNext(seq1) != NULL)
    {
        seq2 = getSeqNext(seq1);
        while (seq2 != NULL)
        {
            alignment = compareSequences(seq1, seq2, m, s, g);
            alignments = (char **)realloc(alignments, sizeof(void *));
            alignments[idx] = alignment;
            seq2 = getSeqNext(seq2);
            ++idx;
        }
        seq1 = getSeqNext(seq1);
    }
    // prints & frees the alignments:
    printAlignment(idx, alignments);
}
/**
 * runs the program (see in file description).
 * @param argc: the number of the program's arguments
 * @param argv: the program arguments.
 * @return: when success- prints the scores of the alignments of all 2 sequences in the file
 *          as mentioned above and exit with 0.
 *          else- prints an informative error massage to the stderr, and exit with EXIT_FAILURE.
 */
int main(const int argc, const char *argv[])
{
    if (argc == 5)
    {
        const char *const fileName = argv[1];
        FILE *file = fopen(fileName, "r");
        if (file != NULL)
        {
            size_t isInteger = 1;
            // get parameters m, s, g :
            int match = (int)stringToInt(argv[2], &isInteger);
            int misMatch = (int)stringToInt(argv[3], &isInteger);
            int gap = (int)stringToInt(argv[4], &isInteger);
            if (isInteger) //assert they are ints
            {
                size_t seqNum = 0;
                Sequence *const sequences = parseSeqFile(file, &seqNum); //part1
                if (seqNum < 2)
                {
                    SEQ_NUM_ERR_MSG(fileName);
                    exit(EXIT_FAILURE);
                }
                calAndPrintScores(sequences, match, misMatch, gap); //part2
                eraseSequences(sequences);
                fclose(file);
                return 0;
            }
        }
    }
    USAGE_ERR_MSG;
    exit(EXIT_FAILURE);
}