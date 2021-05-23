/**
 * @file AnalyzeProtein.c
 * @author  Bar Aloni <baraloni@mail.hujiac.il>
 * @version 1.0
 * @date Nov 1, 2018
 *
 * @brief reads supplied PDB files, analyze the proteins described by them,
 *          and prints the Analyze results.
 *
 * @section DESCRIPTION
 * The system encrypts a msg based on caesar's cipher.
 * Input  : one or moe PDB files.
 * Process: reads the valid atom lines from the supplied PDB files, and analyze
 *          the proteins described by them, one by one. in the analyze process the
 *          program computes the Gravity center, Ionic radius and maximal distance
 *          value of the protein and emits them in th following form:
 *          PDB file <fileName>, <numOfAtomsFound> atoms were read
 *          Cg = <Gravity center.x> <Gravity center.y> <Gravity center.z>
 *          Rg = <Ionic radius>
 *          Dmax = <maximal distance>
 * Output : prints the protein analyze as mentioned above.
 *          when success- exit with 0, else- prints an informative error massage to the
 *          stderr, and exit with EXIT_FAILURE.
 */
//----------------------includes-------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <errno.h>
//----------------------NUMERIC CONSTANTS------------------------------------------------------------
/**
 * represents the maximum num of atoms we are able to analyze in ope protein
 */
#define MAX_NUM_OF_ATOMS 20000
/**
 * represents the minimum length of a valid atom line, i.e: see ATOM_PREFIX
 */
#define MIN_CHARS_IN_ATOM_LINE 60
/**
 * represents the maximum length of a line in a pdb file
 */
#define MAX_CHARS_IN_ATOM_LINE 80
/**
 * represents the num of atom dimensions we will be working on
 */
#define DIMENSIONS 3

/**
 * represents the starting index of the atom line segment that holds the prefix
 */
#define PREFIX_START 0
/**
 * represents the size of the atom line segment that holds the prefix
 */
#define PREFIX_SIZE 6
/**
 * represents the starting index of the atom line segment that holds the atom's x coordinate
 */
#define X_START 30
/**
 * represents the starting index of the atom line segment that holds the atom's y coordinate
 */
#define Y_START 38
/**
 * represents the starting index of the atom line segment that holds the atom's z coordinate
 */
#define Z_START 46
/**
 * represents the size of the atom line segment that holds the coordinate x, y or z val
 */
#define COORDINATE_SIZE 8


//-----------ERRORS SYNTAX-------------------------------------------------------------------------
/**
 * macro that prints an 0 error msg to the stderr.
 * error 0 is to clarify the correct usage
 */
#define USAGE_ERR fprintf(stderr, "Usage: AnalyzeProtein <pdb1> <pdb2> ...\n")
/**
 * macro that prints an 4 error msg to the stderr.
 * error 1 is to notify that an error occurred in opening one of the files
 * (the file that is passed as a param)
 */
#define OPEN_FILE_ERR(fileName) fprintf(stderr, "Error opening file %s.\n", fileName)
/**
 * macro that prints an 2 error msg to the stderr.
 * error 2 is to notify that a pdb file with no atoms was submitted.
 */
#define NO_ATOMS_ERR(fileName) fprintf(stderr, "Error - 0 atoms were found in the file %s.\n",\
                                            fileName)
/**
 * macro that prints an 3 error msg to the stderr.
 * error 3 is to notify when a valid atom line is shorter then the minimal value declared
 * (i.e: in MIN_CHARS_IN_ATOM_LINE)
 */
#define LINE_LENGTH_ERR(len) fprintf(stderr, "ATOM line is too short: %zd characters.\n", len)

//---------------------SYNTAX TO COMPARE-----------------------------------------------------
/**
 * represents a valid atom line prefix
 */
#define ATOM_PREFIX "ATOM  "


//----------------------COMPUTING------------------------------------------------------------------



/**
 * computes and returns the average x, y or z value (determined by the idx parameter) value
 * of the atoms in the supplied array called atoms.
 * @param atoms: 2-dimensional array of size: [<numOfAtoms>][DIMENSIONS]
 * @param numOfAtoms: The atoms array's size
 * @param idx: natural number between 0 and 2, representing the atom's x, y or z value.
 * @return The average value of the atoms x, y or z value (determined by the idx parameter).
 */
float centerOfGravityHelper(const float atoms[][DIMENSIONS], const size_t numOfAtoms, \
                            const size_t idx)
    {
        size_t i;
        float sum = 0;
        for (i = 0; i < numOfAtoms; ++i)
        {
            sum += atoms[i][idx];
        }
        return sum / numOfAtoms;
    }

/**
 * computes the atoms center of gravity and store the result in the supplied  centerOfGravity array.
 * @param atoms: 2-dimensional array of size: [<numOfAtoms>][<DIMENSIONS>]
 * @param numOfAtoms: The atoms array's size
 * @param centerOfGravity: Array of size <DIMENSIONS>.
 */
void calculateCenterOfGravity(const float atoms[][DIMENSIONS], const size_t numOfAtoms, \
                              float centerOfGravity[])
    {
        size_t i;
        for (i = 0; i < DIMENSIONS; ++i)
        {
            centerOfGravity[i] = centerOfGravityHelper(atoms, numOfAtoms, i);
        }
    }

    /**
     * Computes the distance between the 2 given points of <dimension> dimension.
     * @param point1 : a <dimension>-dimensional point
     * @param points2 : a <dimension>-dimensional point
     * @param dimension : the dimension of the supplied points
     * @return the distance between the 2 supplied points
     */
    double distanceBetweenPoints(const float point1[], const float points2[], \
                                 const size_t dimension)
    {
        size_t i;
        double val = 0;
        for (i = 0; i < dimension; ++i)
        {
            val += pow(point1[i] - points2[i], 2);
        }
        return sqrt(val);
    }
/**
 * Computes and returns the ionic radius of the atoms provided.
 * @param atoms: 2-dimensional array of size: [<numOfAtoms>][<DIMENSIONS>].
 * @param numOfAtoms: The atoms array's size.
 * @param centerOfGravity: Array of size <DIMENSIONS>.
 * @return The ionic radius of the atoms provided.
 */
double calculateIonicRadius(const float atoms[][DIMENSIONS], const size_t numOfAtoms, \
                            const float centerOfGravity[])
    {
        size_t i;
        double sum = 0;
        for (i = 0; i < numOfAtoms; ++i)
        {
                const double distFromGravityCenter = \
                            distanceBetweenPoints(atoms[i], centerOfGravity, DIMENSIONS);
                sum += pow(distFromGravityCenter, 2);
        }
        const double average = (sum / numOfAtoms);
        return sqrt(average);
    }

    /**
     * calculate and returns the DMax value of the protein described as atoms[].
     * @param atoms : the protein atoms.
     * @param numOfAtoms : the number of atoms in the supplied atoms array
     * @return DMax value of the protein described as atoms[].
     */
double calculateDMax(const float atoms[][DIMENSIONS], const size_t numOfAtoms)
    {
        size_t i, j;
        double max = 0;
        for (i = 0; i < numOfAtoms; ++i)
        {
            for (j = i + 1; j < numOfAtoms; ++j)
            {
                const double distBetweenPoints = \
                distanceBetweenPoints(atoms[i], atoms[j], DIMENSIONS);
                max = fmax(max, distBetweenPoints);
            }
        }
        return max;
    }


//-----------------------------------PRINTING--------------------------------------------------
/**
 * prints an informative error msg and exits neatly (i.e: closes open file is exists)
 * with failure code.
 * @param errorNum: number from 0 to 3 that correspond to the errors described as macros
 *                  (see documentation).
 * @param file: pointer to the file (NULL if doesn't exist).
 * @param fileName: the file name (NULL if doesn't exist).
 * @param len: the length of the problematic in the file (NULL if doesn't exist).
 * @param coordinate: the problematic coordinate (string rep.) in the file (NULL if doesn't exist).
 */
void printErrorAndExit(int errorNum, FILE *const file, const char * const fileName, \
                       const size_t len)
    {
        switch(errorNum)
        {
            //clarify the correct usage:
            case(0):
                USAGE_ERR;
                break;

            //notify that an error occurred in opening the file that is passed as a param:
            case(1):
                OPEN_FILE_ERR(fileName);
                break;

            //notify that a pdb file with no atoms was submitted:
            case(2):
                NO_ATOMS_ERR(fileName);
                fclose(file);
                break;

            //notify when a valid atom line is shorter then the minimal value declared
            case(3):
                LINE_LENGTH_ERR(len);
                fclose(file);
                break;

            default:
                break;
        }
    exit(EXIT_FAILURE);
    }

/**
 * prints the center of gravity value of the protein.
 * @param atoms: the protein atoms.
 * @param numOfAtoms: the number of atoms in the supplied atoms array
 * @param centerOfGravity: Array of size <DIMENSIONS>.
 */
void printCenterOfGravity(const float atoms[][DIMENSIONS], const size_t numOfAtoms,\
                          float centerOfGravity[])
    {
        calculateCenterOfGravity(atoms, numOfAtoms, centerOfGravity);
        printf("Cg = %.3f %.3f %.3f\n" , centerOfGravity[0], centerOfGravity[1], centerOfGravity[2]);
    }

/**
 * prints the ionic radius value of the protein
 * @param atoms: the protein atoms.
 * @param numOfAtoms : the number of atoms in the supplied atoms array
 * @param centerOfGravity: Array of size <DIMENSIONS>.
 */
void printIonicRadius(const float atoms[][DIMENSIONS], const size_t numOfAtoms,\
                      const float centerOfGravity[])
    {
        double ionicRadius = calculateIonicRadius(atoms, numOfAtoms, centerOfGravity);
        printf("Rg = %.3f\n", ionicRadius);
    }
/**
 * prints the Dmax value of the protein
 * @param atoms: the protein atoms.
 * @param numOfAtoms : the number of atoms in the supplied atoms array
 */
void printDMax(const float atoms[][DIMENSIONS], const size_t numOfAtoms)
    {
        double dMax = calculateDMax(atoms, numOfAtoms);
        printf("Dmax = %.3f\n", dMax);
    }
/**
 * prints the protein's analyze results
 * @param fileName: the file name .
 * @param atoms: the protein atoms.
 * @param numOfAtoms : the number of atoms in the supplied atoms array
 */
void printProteinAnalyze(const char *const fileName, const float atoms[][DIMENSIONS],\
                         const size_t numOfAtoms)
    {
        printf("PDB file %s, %zd atoms were read\n", fileName, numOfAtoms);
        float centerOfGravity[DIMENSIONS] = {0};
        printCenterOfGravity(atoms, numOfAtoms , centerOfGravity);
        printIonicRadius(atoms, numOfAtoms , centerOfGravity);
        printDMax(atoms, numOfAtoms);
    }


//----------------------READING--------------------------------------------------------------------

/**
 * create a new atom from the data in <buff>, and ads it to <atoms> in index <atomCount>.
 * we assume that the data in <buff> is valid (so memcpy and strtof will work correctly)
 * @param atoms: an array to which we add the new atom.
 * @param buff : holds the atom's data.
 * @param atomCount: the index to place the new atom in <atoms>.
 */
void createAtom(float atoms[][DIMENSIONS], const char *const buff, const size_t atomCount)
    {
        char xAsChar[COORDINATE_SIZE + 1] = {'\0'}, yAsChar[COORDINATE_SIZE + 1] = {'\0'},\
                    zAsChar[COORDINATE_SIZE + 1] = {'\0'};
        memcpy(xAsChar, &buff[X_START], COORDINATE_SIZE);
        memcpy(yAsChar, &buff[Y_START], COORDINATE_SIZE);
        memcpy(zAsChar, &buff[Z_START], COORDINATE_SIZE);
        atoms[atomCount][0] = strtof(xAsChar, NULL);
        atoms[atomCount][1] = strtof(yAsChar, NULL);
        atoms[atomCount][2] = strtof(zAsChar, NULL);
    }

/**
 * reads a PDB file and returns the number of atoms that were read from it,
 * and were stored in <atoms>.
 * @param file: a valid pointer to the file.
 * @param atoms: an array.
 * @return: the number of atoms read in the file.
 */
size_t readPDBFile(FILE *const file, float atoms[][DIMENSIONS])
    {
        char buff[MAX_CHARS_IN_ATOM_LINE + 1], prefix[PREFIX_SIZE + 1] = {'\0'};
        char *res = fgets(buff, MAX_CHARS_IN_ATOM_LINE + 1, file);
        size_t atomCount = 0;
        while (res != NULL && atomCount < MAX_NUM_OF_ATOMS) // existing line & room to add more atoms
        {
            memcpy(prefix, &buff[PREFIX_START], PREFIX_SIZE);

            if (!strcmp(prefix, ATOM_PREFIX)) //line starts with "ATOM  "
            {
                if(strlen(res) <= MIN_CHARS_IN_ATOM_LINE) // err 3: ATOM line shorter then 61
                {
                    printErrorAndExit(3, file, NULL, strlen(res));
                }
                createAtom(atoms, buff, atomCount);
                ++atomCount;
            }
            res = fgets(buff, MAX_CHARS_IN_ATOM_LINE + 1, file);
        }
        return atomCount;
    }

//-----------------------------------MAIN------------------------------------------------------
/**
 * runs the program (see in file description).
 * @param argc: the number of the program's arguments
 * @param argv: the program arguments.
 * @return: when success- prints the protein analyze as mentioned above and exit with 0.
 *          else- prints an informative error massage to the stderr, and exit with EXIT_FAILURE.
 */
int main(int argc, const char* argv[])
    {
        if (argc > 1)
        {
            float atoms[MAX_NUM_OF_ATOMS][DIMENSIONS];
            int i;
            for (i = 1; i < argc ; ++i)
            {
                const char *fileName = argv[i];
                FILE *file = fopen(fileName, "r");
                if (file != NULL)
                {
                    //Iv'e assumed that the submitted files are in pdb format, else will result an
                    // unexpected behaviour( i.e: will analyze protein even if the file is not a
                    //pdb file)
                    size_t atomCount = readPDBFile(file, atoms);
                        //err 2: no atoms were detected
                    (!atomCount)? printErrorAndExit(2, file, fileName, (size_t)NULL) : NULL;
                    printProteinAnalyze(fileName, atoms, atomCount);
                    fclose(file);
                    exit(EXIT_SUCCESS);
                }
                printErrorAndExit(1, file, fileName, (size_t)NULL); // err 1: couldn't read file
            }
        }
        printErrorAndExit(0, NULL, NULL, (size_t)NULL); //err 0: wrong usage
    }
