#!/bin/ch
/* Ch package installer script in Ch */
// pkginstall.ch version 1.0.2, April 8, 2005
// *) If a package already installed, after uninstallation,
//    install the new package immediately.
// pkginstall.ch version 1.0.1, April 8, 2005
// *) fixed installation in a dir with blank space
// pkginstall.ch version 1.0.1, April 6, 2005
// *) fixed installation with large number of .chf files in lib
// pkginstall.ch version 1.0, March 23 2005
/* 
     By default, pkginstall installs a package in the current 
     directory  into the <CHHOME>/package and copies required header 
     files in package/include into the CHHOME/toolkit/include  
     directory. It has minimized changes.
  
     If you want to install ch package into your preferred directory,
     you can specify it in the comand line. During the installation,
     it will modify _ipath and _ppath in .chrc in Unix or _chrc 
     in Windows in the  user home  directory.  During installation, 
     an installation file is created under the <CHHOME>/package/installed
     directory with  a list of the installed directories and files. 

     This program can also uninstall a Ch Package by removing header
     files  installed  into  the CHHOME/toolkit/include directory
     and the package in the CHHOME/package directory based on the
     corresponding  package   file  in  CHHOME/package/installed
     directory.

     Usage: pkginstall.ch [-u] [-d directory] PackageName
       -u             uninstall 'packagename'
       -d directory   directory to install 'PackageName'
       Examples:
         (1) uninstallation:
               pkginstall.ch -u chpng
         (2) installation to the default directory:
               pkginstall.ch chpng
         (3) installation to your preferred directory:
               pkginstall.ch -d /your/preferred/directory chpng
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char **argv) {
   char usage[] = 
      "Usage: pkginstall.ch [-u] [-d directory] package\n"
      "       -u             uninstall 'package'\n"
      "       -d directory   directory to install 'package'\n"
      "       Examples:\n" 
      "         (1) uninstallation:\n" 
      "               pkginstall.ch -u chpng \n"
      "         (2) installation to the default directory:\n" 
      "               pkginstall.ch chpng\n"
      "         (3) installation to your preferred directory:\n" 
      "               pkginstall.ch -d /your/preferred/directory chpng\n";
   string_t pkgname;             // chpng
   string_t chhome;              // C:/ch
   string_t chtoolkit;           // C:/ch/toolkit
   string_t chtoolkitinc;        // C:/ch/toolkit/include
   string_t chpackage;           // C:/ch/package
   string_t chpackagename;       // C:/ch/package/chpng
   string_t chpackagenameinc;    // C:/ch/package/chpng/include
   string_t chpackageinstalled;  // C:/ch/package/installed
   string_t chpackageinstallpkg; // C:/ch/package/installed/chpng
   string_t installedfiles;      // files listed in $chpackageinstallpkg
   string_t headerfiles;         // installed header files in $chtoolkitinc 
   int u_option = false;         // -u option
   int d_option = false;         // -d option
   char *s, check;
   string_t cwd = _cwd;
   string_t token;
   string_t str_temp;
   string_t dotchrc;             // .chrc or _chrc
   string_t str_chrc_file;       // contents of ~/.chrc or ~/_chrc
   int install_file_ok = 0;
   int pkg_dir_ok = 0;
   FILE *fptr_installed;
   FILE *fptr_chrc;

   argc--; argv++;
   while(argc>0 && **argv =='-')
   {
      for(s = argv[0]+1; *s&&*s!=' '; s++)  /* empty space not valid option */
      switch(*s)
      {
         case 'u':
            u_option = true;   /* uninstall package */
            break;
         case 'd':             /* the directory to install package */
            d_option = true;
            argc--; argv++;
            chpackage = *argv;
            break;
         default:
            fprintf(stderr,"Warning: invalid option %c\n\n", *s);
            fprintf(stderr,usage);
            exit(1);
            break;
      }
      argc--; argv++;
   } 

   if(argc==1) {            /* get the package name */
     if((*argv)[strlen(*argv)-1] == '/') /* change "chpng/" to "chpng" */
     {
       fprintf(stderr, "Error: invalid package name '%s'\n\n", *argv);
       fprintf(stderr,usage);
       exit(1);
     }
     pkgname = *argv;
   }
   else {
      fprintf(stderr,usage);
      exit(1);
   }

   /* Get installation directories for Ch package */
   chhome=getenv("CHHOME");
   if (chhome==NULL) {
      printf("Sorry, CHHOME cannot be found\n");
      printf("Please install Ch first from http://www.softintegration.com\n");
      exit(0);
   }
   chtoolkit = stradd(chhome, "/toolkit");
   chtoolkitinc = stradd(chhome, "/toolkit/include");
   if(d_option == false) {
     chpackage = stradd(chhome, "/package");
     chpackageinstalled = stradd(chhome, "/package/installed");
     chpackageinstallpkg = stradd(chhome, "/package/installed/", pkgname);
   }
   chpackagename = stradd(chpackage, "/", pkgname);
   chpackagenameinc = stradd(chpackagename, "/include");

   if(access(chpackage, W_OK)) 
   {
      if(u_option == true) {
        fprintf(stderr, "You do not have privilege to remove this package.\n");
        fprintf(stderr, "Please login as 'root' to run this script\n");
        exit(0);
      }
      else if(d_option == true) {
        fprintf(stderr, "You do not have privilege to install this package in '%s'\n", chpackagename);
        exit(0);
      }
      else {
        fprintf(stderr, "You do not have privilege to install this package in '%s'\n", chpackagename);
        fprintf(stderr, "Please login as 'root' to run this script\n");
        fprintf(stderr, "or provide a directory to install this package.\n\n");
        fprintf(stderr,usage);
        exit(0);
      }

   }

   if(u_option)  /* uninstall package */
   {
      // package exists ?
      if(!access(chpackagename, R_OK)) 
        pkg_dir_ok = 1;
      // installation file exists ?
      if(!access(chpackageinstallpkg, R_OK))
        install_file_ok = 1;

      if( pkg_dir_ok && !install_file_ok) {
         fprintf(stderr, "\nPackage '%s' appears to have been installed\n"
           "but the Installation information file does not exist.\n"
           "Would you like to completely remove the directory %s (Y or N)?:" , pkgname, chpackagename);
         check = getchar();
         if (!(check=='y' || check == 'Y')) 
         {
            exit(1);
         } 
         while(getchar() != '\n');
         printf("\n");
         rm -rf $chpackagename
      }
      else if(!pkg_dir_ok && !install_file_ok)
      {
          fprintf(stderr, "\nPackage '%s' was not installed\n", pkgname);
          exit(0);
      }
      else
      {
         if(!pkg_dir_ok)
         {
           fprintf(stderr, "\nThe package directory does not exist but an installation"
		        " file for the\npackage was found.  Removing possible"
	                " remaining files.\n");
         }
         installedfiles = `cat $chpackageinstallpkg`;
         foreach(token; installedfiles)
         {
            rm -rf $token
         }
         rm -rf $chpackageinstallpkg
      }
      printf("\nPackage '%s' has been uninstalled.\n\n", pkgname);
      exit(0);
   }
   /* end uninstall option */

   if(access(pkgname, R_OK))
   {
      fprintf(stderr, "\nPackage '%s' not found in the current diretory.\n\n", pkgname);
      exit(1);
   }

   if(!access(chpackageinstallpkg, R_OK))
   {
      printf("\nPackage '%s' was already installed.\n"
             "Uninstall package '%s'(Y or N)?: ", pkgname, pkgname);
      check = getchar();
      if( check != 'y' && check != 'Y' )
         exit(0);
      while(getchar() != '\n');
      ./pkginstall.ch -u $pkgname
      ./pkginstall.ch $pkgname
      exit(0);
   }
   
   if(!access(chpackagename, R_OK))
   {
      fprintf(stderr, "\nIt appears that the %s package was installed by manually without running\n"
                      "pkginstall.ch or the package directory was not completely removed\n" 
		      "during previous uninstallation.\n"
		      "Overwrite and continue installation (Y or N)?: ", pkgname);
      check = getchar();
      if (!(check=='y' || check == 'Y')) 
      {
         exit(1);
      } 
      while(getchar() != '\n');
	  printf("\n");
   }

   printf("Install package '%s' to %s now, please wait...\n", pkgname, chpackage);
#ifdef _WIN32_
      tar cf $(pkgname).tar $(pkgname)
      cd $chpackage
      tar xf "$cwd/$(pkgname).tar"
      cd $cwd
      rm -f $(pkgname).tar
#else
      tar cf - $(pkgname) | (cd $chpackage; tar xf - )
#endif

   /* install in the default package dir CHHOME/package */
   if(d_option == false) { 
      if(access(chpackageinstalled, R_OK) )
      {
         mkdir $chpackageinstalled
      }  
      if((fptr_installed = fopen(chpackageinstallpkg, "w")) == NULL)
      {
        fprintf(stderr, "\n\nERROR: fopen: cannot open file %s for write.\n\n", chpackageinstallpkg); 
         exit(1);
      }

      fprintf(fptr_installed, "%s\n", chpackagename);
      cd $chpackagenameinc
      headerfiles = `ls`;
      foreach(token; headerfiles)
      {
         cp -rfp $token $chtoolkitinc
         fprintf(fptr_installed, "%s\n", stradd(chtoolkitinc, "/",token));
      }
      fclose( fptr_installed);
   }
   else
   {
#if defined(_WIN32_)
      dotchrc = "_chrc";
#else
      dotchrc = ".chrc";
#endif
      cd ~
      if(access(dotchrc, F_OK)) {
           fprintf(stderr, "\nERROR: ~/'%s' does not exist\n"
	                   "run 'ch -d' to create it.\n");
           exit(0);
      } 
      str_chrc_file = `cat $dotchrc`;
      str_temp = stradd("_ipath=stradd(_ipath,\"",chpackagenameinc, ";\");");
      foreach(token;str_chrc_file;"\0";" ")
      {
        if(!strcmp(token, str_temp))
        {
           token = "found";
           break;
        }
      }
      if(token == NULL)
      {

        if( (fptr_chrc = fopen(dotchrc, "a")) == NULL)
        {
           fprintf(stderr, "\nERROR: fopen: Unable to open ~/%s for append\n", dotchrc);
        } 
        else
        { 
           fprintf(fptr_chrc, "%s\n", str_temp);
           str_temp = stradd("_ppath=stradd(_ppath,\"",chpackage, ";\");");
           fprintf(fptr_chrc, "%s\n", str_temp);
           fclose( fptr_chrc);
        }
      }
      cd -
   }

   printf("Package '%s' has been successfully installed.\n\n", pkgname);
   if(!access(stradd(chpackagename, "/demos"), R_OK)) {
     printf("Please try demo programs for this package in \n"
          "%s\n\n", stradd(chpackagename, "/demos")); 
   }
   if(!access("postinstall.ch", R_OK))
   {
      printf("You must run the post installation program,\n"
             "   %s\nbefore running any demos.\n\n", "postinstall.ch");
   }

   return 0;
}
