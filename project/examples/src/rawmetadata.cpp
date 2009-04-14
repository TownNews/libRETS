/*
 * Copyright (C) 2005 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */

#include "librets.h"
#include "Options.h"
#include <iostream>

using namespace librets;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::exception;
namespace po = boost::program_options;

void dumpSystem(RetsMetadata * metadata);
void dumpAllResources(RetsMetadata * metadata);
void dumpAllClasses(RetsMetadata * metadata, MetadataResource * resource);
void dumpAllTables(RetsMetadata * metadata, MetadataClass * aClass);
void dumpAllLookups(RetsMetadata * metadata, MetadataResource * resource);
void dumpAllLookupTypes(RetsMetadata * metadata, MetadataLookup * lookup);

int main(int argc, char * argv[])
{
    try
    {
        Options options;
        string outputFilename;

        options.descriptions.add_options()
            ("output-file", po::value<string>(&outputFilename)
             ->default_value("rawmetadata.xml"), "Output file name")
            ;
        if (!options.ParseCommandLine(argc, argv))
        {
            return 0;
        }

        RetsSessionPtr session = options.RetsLogin();
        if (!session)
        {
            cout << "Login failed\n";
            return -1;
        }

        /*
         * Perform the search, returning the results to the output file.
         */
        std::ofstream outputStream(outputFilename.c_str());
        session->GetMetadata(outputStream);
        outputStream.close();
        
        /*
         * We're done with the session so we can log out.
         */
        session->Logout();

        /*
         * Reopen the file for input.
         */
        istreamPtr inputStream(new std::ifstream(outputFilename.c_str()));

        /*
         * Create the RetsMetadata object to handle the parsing of the data.
         */
        RetsMetadata * metadata = RetsMetadata::CreateAndParse(inputStream, options.getEncoding());
        
        dumpSystem(metadata);
        dumpAllResources(metadata);

    }
    catch (RetsException & e)
    {
        e.PrintFullReport(cout);
        return 1;
    }
    catch (exception & e)
    {
        cout << e.what() << endl;
        return 2;
    }
    return 0;
}

void dumpSystem(RetsMetadata * metadata)
{
    MetadataSystem * system = metadata->GetSystem();
    cout << "System ID: " << system->GetSystemID() << endl;
    cout << "System Description: " << system->GetSystemDescription() << endl;
    cout << "Comments: " << system->GetComments() << endl;
}

void dumpAllResources(RetsMetadata * metadata)
{
    MetadataResourceList resources = metadata->GetAllResources();
    MetadataResourceList::iterator i;
    cout << endl;
    for (i = resources.begin(); i != resources.end(); i++)
    {
        MetadataResource * resource = *i;
        dumpAllClasses(metadata, resource);
    }

    for (i = resources.begin(); i != resources.end(); i++)
    {
        MetadataResource * resource = *i;
        dumpAllLookups(metadata, resource);
    }
}

void dumpAllClasses(RetsMetadata * metadata, MetadataResource * resource)
{
    string resourceName = resource->GetResourceID();
    
    MetadataClassList classes =
        metadata->GetAllClasses(resourceName);
    MetadataClassList::iterator i;
    for (i = classes.begin(); i != classes.end(); i++)
    {
        MetadataClass * aClass = *i;
        cout << "Resource name: " << resourceName << " ["
             << resource->GetStandardName() << "]" << endl;
        cout << "Class name: " << aClass->GetClassName() << " ["
             << aClass->GetStandardName() << "]" << endl;
        dumpAllTables(metadata, aClass);
        cout << endl;
    }
}

void dumpAllTables(RetsMetadata * metadata, MetadataClass * aClass)
{
    MetadataTableList tables = metadata->GetAllTables(aClass);
    MetadataTableList::iterator i;
    for (i = tables.begin(); i != tables.end(); i++)
    {
        MetadataTable * table = *i;
        cout << "Table name: " << table->GetSystemName() << " ["
             << table->GetStandardName() << "]" << " ("
             << table->GetDataType() << ")";
        if (!table->GetMetadataEntryID().empty())
        {
            cout << " MetadataEntryID: " << table->GetMetadataEntryID();
        }
        cout << endl;
    }
}

void dumpAllLookups(RetsMetadata * metadata, MetadataResource * resource)
{
    string resourceName = resource->GetResourceID();
    
    MetadataLookupList classes =
        metadata->GetAllLookups(resourceName);
    MetadataLookupList::iterator i;
    for (i = classes.begin(); i != classes.end(); i++)
    {
        MetadataLookup * lookup = *i;
        cout << "Resource name: " << resourceName << " ["
             << resource->GetStandardName() << "]" << endl;
        cout << "Lookup name: " << lookup->GetLookupName() << " ("
             << lookup->GetVisibleName() << ")";

        if (!lookup->GetMetadataEntryID().empty())
        {
            cout << " MetadataEntryID: " << lookup->GetMetadataEntryID();
        }
            
        cout << endl;
        dumpAllLookupTypes(metadata, lookup);
        cout << endl;
    }
}

void dumpAllLookupTypes(RetsMetadata * metadata, MetadataLookup * lookup)
{
    MetadataLookupTypeList lookupTypes = metadata->GetAllLookupTypes(lookup);
    MetadataLookupTypeList::const_iterator i ;
    for (i = lookupTypes.begin(); i != lookupTypes.end(); i++)
    {
        MetadataLookupType * lookupType = *i;
        cout << "Lookup value: " << lookupType->GetValue() << " ("
             << lookupType->GetShortValue() << ", "
             << lookupType->GetLongValue() << ")"; 

        if (!lookupType->GetMetadataEntryID().empty())
        {
            cout << " MetadataEntryID: " << lookupType->GetMetadataEntryID();
        }
       
        cout << endl;
    }
}