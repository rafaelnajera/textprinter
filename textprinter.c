/*

textprinter.c

*/
/* This is a change just to test Netbeans' integration with Github */

#include <cairo/cairo.h>
#include <cairo/cairo-pdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINESIZE 1000
#define MINTEXTFONTSIZE 8
#define DEFAULTTEXTFONTSIZE 8

char *usage = "Usage:\n   textprinter [-h]\n"
              "        [-i <input file>] [-o <output file] [-t title] [-s fontsize]\n"
              "        [--singlespace] [--semidoublespace] [--doublespace]\n"
              "        [--nolinenumbers]\n";
              

void chopNewLine(char *str)
// take the \n at the end of a line, if exists
{
   
   int l;
   
   l = strlen(str);
   
   if (str[l-1] == '\n')
      str[l-1] = '\0';
}

int main(int argc, char *argv[]) {

   cairo_surface_t *surface;
   cairo_t *cr;
   cairo_text_extents_t extents;
   
   int x,y, linesPerPage;
   char *line, *lineToPrint, *headerToPrint;  
   int fileLineNumber, pageLineNumber, pageNumber;

   int marginTop = 72;
   int marginLeft = 36;
   int marginBottom = 36;
   int marginRight = 36;
   int pageWidth = 612;  // 8.5 in
   int pageHeight = 792; // 11 in

   const char *textFontName = "Monospace";
   int textFontSize = DEFAULTTEXTFONTSIZE;
   int lineHeight = 20;
   const char *titleFontName = "Sans";
   const int titleFontSize = 10;
   const char *titleFormat = "%s    ( %d )";

   char *pageTitle;
   char *outFileName, *inFileName;
   char *defaultPageTitle = "stdin";
   char *defaultOutFileName = "printout.pdf";
   char *defaultInFileName = "-";
   FILE *inFile;
   
   int printLineNumbers = 1;

   int argi;
   
   pageTitle = defaultPageTitle;
   outFileName = defaultOutFileName;
   inFileName = defaultInFileName;

   // parse arguments   
   argi = 1;
   if (argc > 1){
      while( (argi < argc) && (argv[argi][0] == '-')){
        if (argv[argi][1] == 'h'){
            fputs(usage, stderr);
            return 0;
         }
         else
         if (argv[argi][1] == 'o'){
            if (argc == (argi+1)){
               fprintf(stderr, "Please give an output file name\n");
               return 0;
            }
            else {
               outFileName = argv[argi+1];
               argi++;
            }
         }
         if (argv[argi][1] == 'i'){
            if (argc == (argi+1)){
               fprintf(stderr, "Please give an input file name\n");
               return 0;
            }
            else {
               inFileName = argv[argi+1];
               argi++;
            }
         }
         if (argv[argi][1] == 's'){
            if (argc == (argi+1)){
               fprintf(stderr, "Please give a font size\n");
               return 0;
            }
            else {
               textFontSize = atoi(argv[argi+1]);
               if (textFontSize < MINTEXTFONTSIZE)
                  textFontSize = MINTEXTFONTSIZE; 
               argi++;
            }
         }
         if (argv[argi][1] == 't'){
            if (argc == (argi+1)){
               fprintf(stderr, "Please give a title\n");
               return 0;
            }
            else {
               pageTitle = argv[argi+1];
               argi++;
            }
         }
         else
         if (strcmp(argv[argi], "--singlespace")==0){
            lineHeight = 11;
         }
         else
         if (strcmp(argv[argi], "--semidoublespace")==0){
            lineHeight = 15;
         }
         else
         if (strcmp(argv[argi], "--nolinenumbers")==0){
            printLineNumbers = 0;
         };
         
         argi++;
      }
     
   } 

   if (strcmp(inFileName, "-") == 0){
      inFile = stdin;
      fprintf(stderr, "Reading from stdin, writing to %s\n", outFileName);
   } 
   else {
      inFile = fopen(inFileName, "r");
      if (inFile == NULL){
         fprintf(stderr, "Error: can't open input file '%s'\n", inFileName);
         return 0;
      }
      fprintf(stderr, "Reading from %s, writing to %s\n", inFileName, 
              outFileName);
   }
   surface = cairo_pdf_surface_create(outFileName, pageWidth, pageHeight);
   cr = cairo_create(surface);
   cairo_set_source_rgb(cr, 0, 0, 0);
  
   linesPerPage = (pageHeight - marginTop - marginBottom) / lineHeight;   
   line = malloc(MAXLINESIZE);
   lineToPrint = malloc(MAXLINESIZE+4);
   headerToPrint = malloc(80);
   pageNumber = 1;
   fileLineNumber = 0;
   pageLineNumber = 0;
   line = fgets(line,MAXLINESIZE,inFile);
   y = marginTop;
   cairo_select_font_face (cr, textFontName, CAIRO_FONT_SLANT_NORMAL,
                              CAIRO_FONT_WEIGHT_NORMAL);
   cairo_set_font_size (cr, textFontSize);
    
   while(!feof(inFile)){
      fileLineNumber++;
      cairo_move_to(cr, marginLeft, y);
      chopNewLine(line);
      if (printLineNumbers)
         sprintf(lineToPrint, "%4d   %s", fileLineNumber, line);
      else
         strcpy(lineToPrint, line);
      cairo_show_text(cr, lineToPrint);
      
      if (pageLineNumber < linesPerPage){
         y += lineHeight;
         pageLineNumber++;
      } 
      else { 
         // print header & turn the page
         cairo_select_font_face (cr, titleFontName, CAIRO_FONT_SLANT_NORMAL,
                                    CAIRO_FONT_WEIGHT_NORMAL);
         cairo_set_font_size (cr, titleFontSize);
         sprintf(headerToPrint, titleFormat, pageTitle, pageNumber);
         cairo_text_extents(cr, headerToPrint, &extents);
         cairo_move_to(cr, pageWidth - marginRight - extents.width, 
                        marginTop / 2);
         cairo_show_text(cr, headerToPrint);

         cairo_select_font_face (cr, textFontName, CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_NORMAL);
         cairo_set_font_size (cr, textFontSize);
         y = marginTop;
         pageLineNumber = 0;
         cairo_show_page(cr);
         pageNumber++;
      }
      line = fgets(line,MAXLINESIZE,inFile);
   }

   // print last header 
   cairo_select_font_face (cr, titleFontName, CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_NORMAL);
   cairo_set_font_size (cr, titleFontSize);
   sprintf(headerToPrint, titleFormat, pageTitle, pageNumber);
   cairo_text_extents(cr, headerToPrint, &extents);
   cairo_move_to(cr, pageWidth - marginRight - extents.width, marginTop / 2);
   cairo_show_text(cr, headerToPrint); 

   // finish up
   cairo_surface_destroy(surface);
   cairo_destroy(cr);
   fclose(inFile);

   return 0;
}
