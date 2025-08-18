#ifndef MESSAGE_TYPE_H 
#define MESSAGE_TYPE_H 
typedef enum {
	Audio,
	Announcement,
} MsgType;
typedef struct {
	MsgType Type;
	char Data[1024];
} Message;
// Contents of the header Vfile (declarations, definitions, etc.)

#endif // MY_HEADER_FILE_Henum MsgType{

