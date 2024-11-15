//******************************************************************
//*  ██╗  ██╗██╗██████╗  ██████╗     ██╗  ██╗    ██████╗
//*  ██║  ██║██║██╔══██╗██╔═══██╗    ██║  ██║   ██╔═████╗
//*  ███████║██║██████╔╝██║   ██║    ███████║   ██║██╔██║
//*  ██╔══██║██║██╔═══╝ ██║   ██║    ╚════██║   ████╔╝██║
//*  ██║  ██║██║██║     ╚██████╔╝         ██║██╗╚██████╔╝
//*  ╚═╝  ╚═╝╚═╝╚═╝      ╚═════╝          ╚═╝╚═╝ ╚═════╝
//************************ Jefferson National Lab (2017) ***********
//******************************************************************
//* Example program for reading HIPO-4 Files..
//* Reads the file created by writeFile program
//*--
//* Author: G.Gavalian
//*

#include <cstdlib>
#include <iostream>
#include "reader.h"



int main(int argc, char** argv) {

   std::cout << " reading file example program (HIPO) "  << __cplusplus << std::endl;

   char inputFile[256];
   char inputBank[256];

   if(argc>2) {
     snprintf(inputFile,256,"%s",argv[1]);
     snprintf(inputBank,256,"%s",argv[2]);
      //sprintf(outputFile,"%s",argv[2]);
   } else {
      std::cout << " *** please provide a file name..." << std::endl;
      std::cout << "     Syntaxe : ./showFile.exe FILE.hipo BANK_NAME" << std::endl;
      std::cout << "     Example of use : ./showFile.exe alert_sim_10k.hipo MC::Particle" << std::endl;
     exit(1);
   }

   hipo::reader  reader;
   reader.open(inputFile);
   hipo::dictionary  factory;
   reader.readDictionary(factory);

   factory.show();

   hipo::bank  inBank(factory.getSchema(inputBank));

   hipo::event      event;

   int counter = 0;

   //reader.gotoEvent(561);

   while(reader.next()==true){
      if (counter >3) {
         break;
      }
      reader.read(event);
      event.getStructure(inBank);
      std::cout << "#EVENT : " << counter +1 << std::endl;
      inBank.show();
      counter++;
   }
   printf("processed events = %d\n",counter);
}
//### END OF GENERATED CODE
