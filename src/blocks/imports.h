struct SWFImportBlock_s
{
	struct SWFBlock_s block;
	char *filename;
	struct importitem
	{	struct importitem *next;
		int id;
		char *name;
	} *importlist;
};
typedef struct SWFImportBlock_s *SWFImportBlock;

SWFImportBlock newSWFImportBlock(const char *filename);

