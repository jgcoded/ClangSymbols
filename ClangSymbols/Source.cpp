#include <iostream>
#include <string>

extern "C" {
#include <clang-c/Index.h>
}

#define TESTFILE_NAME "test.cpp"

using namespace std;

struct ClientData
{
    string sourceFilename;
};

CXChildVisitResult visit(CXCursor cursor, CXCursor parent, CXClientData data)
{
    ClientData* clientData = static_cast<ClientData*>(data);
    CXFile file;
    unsigned int line, column, offset;
    clang_getInstantiationLocation(
        clang_getCursorLocation(cursor),
        &file, &line, &column, &offset);

    CXString cFilename = clang_getFileName(file);
    string cursorFilename(clang_getCString(cFilename));

    // avoid nodes going into other files
    if (cFilename.data == nullptr || cursorFilename != clientData->sourceFilename) {
        clang_disposeString(cFilename);
        return CXChildVisit_Continue;
    }
    clang_disposeString(cFilename);

    CXString displayNameCXStr = clang_getCursorDisplayName(cursor);
    string displayName(clang_getCString(displayNameCXStr));
    clang_disposeString(displayNameCXStr);

    CXString cursorSpellingCXStr = clang_getCursorSpelling(cursor);
    string cursorSpelling(clang_getCString(cursorSpellingCXStr));
    clang_disposeString(cursorSpellingCXStr);
    
    CXString cursorUSRCXStr = clang_getCursorUSR(cursor);
    string cursorUSR(clang_getCString(cursorUSRCXStr));
    clang_disposeString(cursorUSRCXStr);

    CXCursorKind kind = clang_getCursorKind(cursor);
    CXString kindCXStr = clang_getCursorKindSpelling(kind);
    string kindStr(clang_getCString(kindCXStr));
    clang_disposeString(kindCXStr);

    cout << "Visiting node: " << endl
        << "display name: " << displayName << endl
        << "cursor spelling: " << cursorSpelling << endl
        << "cursor USR: " << cursorUSR << endl
        << "kind: " << kindStr << endl
        << "Line: " << line << endl
        << "column: " << column << endl
        << "offset: " << offset << endl 
        << "file: " << cursorFilename << endl << endl;

    return CXChildVisit_Recurse;
}

int main(int argc, const char* argv[])
{
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit translationUnit = clang_parseTranslationUnit(index, TESTFILE_NAME, nullptr, 0, nullptr, 0, CXTranslationUnit_None);

    int numDiagnostics = clang_getNumDiagnostics(translationUnit);
    cout << "Number of diagnostic messages: " << numDiagnostics << endl;

    if (numDiagnostics > 0)
    {
        for (int i = 0; i < numDiagnostics; ++i)
        {
            CXDiagnostic diagnostic = clang_getDiagnostic(translationUnit, i);
            CXString diagnosticStr = clang_formatDiagnostic(diagnostic, clang_defaultDiagnosticDisplayOptions());
            cout << clang_getCString(diagnosticStr) << endl;
            clang_disposeString(diagnosticStr);
            clang_disposeDiagnostic(diagnostic);
        }
    }

    ClientData data;
    data.sourceFilename = TESTFILE_NAME;
    clang_visitChildren(clang_getTranslationUnitCursor(translationUnit), &visit, &data);

    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);

    getchar();

    return 0;
}
