#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include "kpr.h"
#include "kprBehavior.h"
#include "kprContent.h"
#include "kprShell.h"
#include "kprURL.h"

#include <gtk/gtk.h>


static xsSlot callback;

void KPR_system_alert(xsMachine* the)
{
	int argc = xsToInteger(xsArgc);
	KprShell self = gShell;
	/*
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];
	xsStringValue string;
	if ((argc > 0) && xsTest(xsArg(0))) {
		if (xsFindString(xsArg(0), xsID_type, &string)) {
			if (!FskStrCompare(string, "about"))
				[alert setAlertStyle:NSInformationalAlertStyle];
			else if (!FskStrCompare(string, "stop"))
				[alert setAlertStyle:NSCriticalAlertStyle];
			else if (!FskStrCompare(string, "note"))
				[alert setAlertStyle:NSWarningAlertStyle];
		}
		if (xsFindString(xsArg(0), xsID_prompt, &string))
			[alert setMessageText:[NSString stringWithUTF8String:string]];
		if (xsFindString(xsArg(0), xsID_info, &string))
			[alert setInformativeText:[NSString stringWithUTF8String:string]];
		if (xsFindResult(xsArg(0), xsID_buttons)) {
			if (xsIsInstanceOf(xsResult, xsArrayPrototype)) {
				xsIntegerValue c = xsToInteger(xsGet(xsResult, xsID_length)), i;
				for (i = 0; i < c; i++) {
					string = xsToString(xsGet(xsResult, i));
					[alert addButtonWithTitle:[NSString stringWithUTF8String:string]];
				}
			}
		}
	}
	*/
	if ((argc > 1) && xsTest(xsArg(1)))
		callback = xsArg(1);
	else
		callback = xsNull;
	xsRemember(callback);
	(*self->handler)(NULL, kFskEventWindowDeactivated, NULL, self);
	/*
	[alert beginSheetModalForWindow:[NSApp mainWindow] completionHandler:^(NSInteger result) {
		(*self->handler)(NULL, kFskEventWindowActivated, NULL, self);
		[alert.window close]; 
		xsBeginHostSandboxCode(self->the, self->code);
		{
			xsVars(1);
			{
				xsTry {
					xsResult = xsAccess(callback);
					xsForget(callback);
					callback = xsNull;
					if (xsTest(xsResult)) {
						xsVar(0) = (result == 1000) ? xsTrue : (result == 1001) ? xsUndefined : xsFalse;
						(void)xsCallFunction1(xsResult, xsNull, xsVar(0));
					}
				}
				xsCatch {
				}
			}
		}
		xsEndHostSandboxCode();
	}];
	*/
}

void KPR_system_getClipboardText(xsMachine* the)
{
	gdk_threads_enter();
	GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (gtk_clipboard_wait_is_text_available(clipboard)) {
		char *utf8Ptr = gtk_clipboard_wait_for_text(clipboard);
		FskDebugStr("KPR_system_getClipboardText '%s'\n", utf8Ptr);
		if (utf8Ptr) {
			xsResult = xsString(utf8Ptr);
			g_free(utf8Ptr);
			goto bail;
		}
	}
	xsResult = xsString("");
bail:
	gdk_threads_leave();
}

void KPR_system_setClipboardText(xsMachine* the)
{
	gdk_threads_enter();
	GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	if (xsToInteger(xsArgc) > 0) {
		char* utf8Ptr = xsToString(xsArg(0));
		gtk_clipboard_set_text(clipboard, utf8Ptr, FskStrLen(utf8Ptr));
	}
	else
		gtk_clipboard_set_text(clipboard, "", 0);
	gdk_threads_leave();
}

/*
@class KPR_system_open_delegate;
@interface KPR_system_open_delegate : NSObject <NSOpenSavePanelDelegate> {
    NSString *_name;
}
- (instancetype)initWith:(NSString *)name;
- (void)dealloc;
- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url;
@end
@implementation KPR_system_open_delegate {
}
- (instancetype)initWith:(NSString *)name {
	self = [super init];
	if (self) {
		_name = [name retain];
	}
	return self;
}
- (void)dealloc {
    [_name release];
    [super dealloc];
}
- (BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url {
    NSNumber *isDirectory;
    [url getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:nil];
	return [isDirectory boolValue] || [_name isEqualToString:[url lastPathComponent]];
}
@end
*/

static void KPR_system_open(xsMachine* the, xsBooleanValue flag)
{
	int argc = xsToInteger(xsArgc);
	KprShell self = gShell;
	xsStringValue string;

	GtkWidget *dialog;
	GtkFileChooserAction action = flag ? GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER : GTK_FILE_CHOOSER_ACTION_OPEN;
	gint res;
	char *promptString = NULL;
	char *msgString = NULL;
	char *uriString = NULL;

	if ((argc > 0) && xsTest(xsArg(0))) {
		if (xsFindString(xsArg(0), xsID_fileType, &string))
			//[openPanel setAllowedFileTypes: [NSArray arrayWithObject:[NSString stringWithUTF8String:string]]];
			;
		if (xsFindString(xsArg(0), xsID_message, &string))
			msgString = FskStrDoCopy(string);
		if (xsFindString(xsArg(0), xsID_name, &string))
			//[openPanel setDelegate:[[KPR_system_open_delegate alloc] initWith:[NSString stringWithUTF8String:string]]];
			;
		if (xsFindString(xsArg(0), xsID_prompt, &string))
			promptString = FskStrDoCopy(string);
		if (xsFindString(xsArg(0), xsID_url, &string))
			uriString = FskStrDoCopy(string);
	}

	dialog = gtk_file_chooser_dialog_new (msgString, NULL, action, 
		"Cancel", GTK_RESPONSE_CANCEL,
		promptString, GTK_RESPONSE_ACCEPT,
		NULL);

	if (uriString)
		gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(dialog), uriString);

	gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(dialog), FALSE);

	if ((argc > 1) && xsTest(xsArg(1)))
		callback = xsArg(1);
	else
		callback = xsNull;
	xsRemember(callback);
	(*self->handler)(NULL, kFskEventWindowDeactivated, NULL, self);

	res = gtk_dialog_run (GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *uri;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		uri = gtk_file_chooser_get_uri(chooser);
		printf("KPR_system_open %s\n", uri);
		
		(*self->handler)(NULL, kFskEventWindowActivated, NULL, self);
		xsBeginHostSandboxCode(self->the, self->code);
		{
			xsVars(1);
			{
				xsTry {
					xsResult = xsAccess(callback);
					xsForget(callback);
					callback = xsNull;
					if (xsTest(xsResult)) {
						if (uri)
							xsVar(0) = xsString((xsStringValue)uri);
						(void)xsCallFunction1(xsResult, xsNull, xsVar(0));
					}
				}
				xsCatch {
				}
			}
		}
		xsEndHostSandboxCode();

		g_free(uri);
	}

	gtk_widget_destroy(dialog);

	FskMemPtrDispose(msgString);
	FskMemPtrDispose(promptString);
	FskMemPtrDispose(uriString);
}

void KPR_system_openDirectory(xsMachine* the)
{
	KPR_system_open(the, 1);
}

void KPR_system_openFile(xsMachine* the)
{
	KPR_system_open(the, 0);
}

void KPR_system_save(xsMachine* the, xsBooleanValue flag)
{
	xsIntegerValue argc = xsToInteger(xsArgc);
	KprShell self = gShell;
	xsStringValue string;

	GtkWidget *dialog;
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
	gint res;
	char *promptString = NULL;
	char *msgString = NULL;
	char *nameString = NULL;
	char *uriString = NULL;
	
	if ((argc > 0) && xsTest(xsArg(0))) {
		if (xsFindString(xsArg(0), xsID_name, &string))
			nameString = FskStrDoCopy(string);
		if (xsFindString(xsArg(0), xsID_prompt, &string))
			promptString = FskStrDoCopy(string);
		if (xsFindString(xsArg(0), xsID_url, &string))
			uriString = FskStrDoCopy(string);
	}

	if ((argc > 1) && xsTest(xsArg(1)))
		callback = xsArg(1);
	else
		callback = xsNull;
	xsRemember(callback);

	dialog = gtk_file_chooser_dialog_new (msgString, NULL, action, 
			"Cancel", GTK_RESPONSE_CANCEL,
			promptString, GTK_RESPONSE_ACCEPT,
			NULL);

	if (uriString)
		gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(dialog), string);

	if (nameString)
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(dialog), nameString);

	(*self->handler)(NULL, kFskEventWindowDeactivated, NULL, self);

	res = gtk_dialog_run (GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT)
	{
		char *uri;
		GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
		uri = gtk_file_chooser_get_uri(chooser);
		printf("KPR_system_save %s\n", uri);

		(*self->handler)(NULL, kFskEventWindowActivated, NULL, self);
		xsBeginHostSandboxCode(self->the, self->code);
		{
			xsVars(1);
			{
				xsTry {
					xsResult = xsAccess(callback);
					xsForget(callback);
					callback = xsNull;
					if (xsTest(xsResult)) {
						if (uri)
							xsVar(0) = xsString((xsStringValue)uri);
						(void)xsCallFunction1(xsResult, xsNull, xsVar(0));
					}
				}
				xsCatch {
				}
			}
		}
		xsEndHostSandboxCode();

		g_free(uri);
	}

	gtk_widget_destroy(dialog);

	FskMemPtrDispose(nameString);
	FskMemPtrDispose(msgString);
	FskMemPtrDispose(promptString);
	FskMemPtrDispose(uriString);
}


void KPR_system_saveDirectory(xsMachine* the)
{
	KPR_system_save(the, 1);
}

void KPR_system_saveFile(xsMachine* the)
{
	KPR_system_save(the, 0);
}

void KPR_system_beginModal(xsMachine* the)
{
	KprShell self = gShell;
	(*self->handler)(NULL, kFskEventWindowDeactivated, NULL, self);
}

void KPR_system_endModal(xsMachine* the)
{
	KprShell self = gShell;
	(*self->handler)(NULL, kFskEventWindowActivated, NULL, self);
}

void KPR_system_getWifiInfo(xsMachine* the)
{
	/*
	CWInterface *currentInterface = [CWInterface interface];
	if (currentInterface) {
		const char* ssid = [currentInterface.ssid UTF8String];
		if (ssid) {
			const char* interfaceName = [currentInterface.interfaceName UTF8String];
			xsResult = xsNewInstanceOf(xsObjectPrototype);
			xsSet(xsResult, xsID("SSID"), xsString((xsStringValue)ssid));
			xsSet(xsResult, xsID("interfaceIndex"), xsInteger(if_nametoindex(interfaceName)));
			xsSet(xsResult, xsID("interfaceName"), xsString((xsStringValue)interfaceName));
		}
	}
	*/
}

void KPR_system_networkInterfaceIndexToName(xsMachine* the)
{
	char name[IF_NAMESIZE];
	UInt32 index = xsToInteger(xsArg(0));
	if (if_indextoname(index, name))
		xsResult = xsString((xsStringValue)name);
}

typedef struct {
	xsMachine* the;
	xsSlot slot;
	xsIntegerValue status;
	//NSTask* task;
	xsIntegerValue usage;
} KprShellExecRecord, *KprShellExec;

void KPR_shell_execute_callback(KprShellExec exec)
{
	exec->usage--;
	if (exec->usage == 0) {
		xsBeginHost(exec->the);
		xsVars(3);
		xsVar(0) = xsAccess(exec->slot);
		xsVar(1) = xsGet(xsVar(0), xsID_callback);
		if (xsTest(xsVar(1))) {
			xsVar(2) = xsInteger(exec->status);
			xsResult = xsCallFunction1(xsVar(1), xsNull, xsVar(2));
		}
		xsForget(exec->slot);
		xsSetHostData(xsVar(0), NULL);
		xsEndHost(exec->the);
		FskMemPtrDispose(exec);
	}
}

void KPR_shell_execute_cancel(xsMachine* the)
{
	//KprShellExec exec = xsGetHostData(xsThis);
	/*
	NSTask *task = exec->task;
	[task terminate];
	*/
}

void KPR_shell_execute_destructor(void* data)
{
}

void KPR_shell_execute_stderr(KprShellExec exec, char *string)
{
	if (string) {
		xsBeginHost(exec->the);
		xsVars(3);
		xsVar(0) = xsAccess(exec->slot);
		xsVar(1) = xsGet(xsVar(0), xsID_stderr);
		if (xsTest(xsVar(1))) {
			xsVar(2) = xsString(string);
			xsResult = xsCallFunction1(xsVar(1), xsNull, xsVar(2));
		}
		xsEndHost(exec->the);
		FskMemPtrDispose(string);
	}
	KPR_shell_execute_callback(exec);
}

void KPR_shell_execute_stdout(KprShellExec exec, char *string)
{
	if (string) {
		xsBeginHost(exec->the);
		xsVars(3);
		xsVar(0) = xsAccess(exec->slot);
		xsVar(1) = xsGet(xsVar(0), xsID_stdout);
		if (xsTest(xsVar(1))) {
			xsVar(2) = xsString(string);
			xsResult = xsCallFunction1(xsVar(1), xsNull, xsVar(2));
		}
		xsEndHost(exec->the);
		FskMemPtrDispose(string);
	}
	KPR_shell_execute_callback(exec);
}

void KPR_shell_execute(xsMachine* the)
{
	KprShellExec exec = NULL;
	// NSTask *task;
// 	NSFileHandle *stderrFile = NULL;
// 	NSFileHandle *stdoutFile = NULL;
	xsVars(5);
	
	xsThrowIfFskErr(FskMemPtrNewClear(sizeof(KprShellExecRecord), &exec));
	xsVar(0) = xsNewHostObject(KPR_shell_execute_destructor);
	exec->the = the;
	exec->slot = xsVar(0);
	xsSetHostData(xsVar(0), exec);
	xsRemember(exec->slot);
	
	xsResult = xsNewHostFunction(KPR_shell_execute_cancel, 0);
	xsNewHostProperty(xsVar(0), xsID_cancel, xsResult, xsDefault, xsDontDelete | xsDontEnum | xsDontSet);
	
// 	task = exec->task = [[NSTask alloc] init];
// 	[task setLaunchPath:@"/bin/sh"];
// 	NSArray *arguments = [NSArray arrayWithObjects: @"-c", [NSString stringWithUTF8String:xsToString(xsArg(0))], NULL];
// 	[task setArguments:arguments];

	if (xsFindResult(xsArg(1), xsID_callback)) {
		xsNewHostProperty(xsVar(0), xsID_callback, xsResult, xsDefault, xsDontDelete | xsDontEnum | xsDontSet);
	}
	else {
		xsNewHostProperty(xsVar(0), xsID_callback, xsNull, xsDefault, xsDontDelete | xsDontEnum | xsDontSet);
	}
	// if (xsFindResult(xsArg(1), xsID_directory)) {
// 		NSString *path = [NSString stringWithUTF8String:xsToString(xsResult)];
// 		[task setCurrentDirectoryPath:path];
// 	}
// 	if (xsFindResult(xsArg(1), xsID_environment)) {
// 		NSMutableDictionary *environment = [NSMutableDictionary dictionaryWithCapacity:16];
// 		xsVar(1) = xsEnumerate(xsResult);
// 		for (;;) {
// 			xsVar(2) = xsCall0(xsVar(1), xsID("next"));
// 			xsVar(3) = xsGet(xsVar(2), xsID("done"));
// 			if (xsTest(xsVar(3)))
// 				break;
// 			xsVar(3) = xsGet(xsVar(2), xsID("value"));
// 			xsVar(4) = xsGetAt(xsResult, xsVar(3));
// 			NSString *key = [NSString stringWithUTF8String:xsToString(xsVar(3))];
// 			NSString *object = [NSString stringWithUTF8String:xsToString(xsVar(4))];
// 			[environment setObject:object forKey:key];
// 		}
// 		[task setEnvironment:environment];
// 	}
// 	if (xsFindResult(xsArg(1), xsID_stderr)) {
// 		xsNewHostProperty(xsVar(0), xsID_stderr, xsResult, xsDefault, xsDontDelete | xsDontEnum | xsDontSet);
// 		NSPipe *pipe = [NSPipe pipe];
// 		[task setStandardError:pipe];
// 		stderrFile = [pipe fileHandleForReading];
// 	}
// 	else {
// 		xsNewHostProperty(xsVar(0), xsID_stderr, xsNull, xsDefault, xsDontDelete | xsDontEnum | xsDontSet);
// 	}
// 	if (xsFindResult(xsArg(1), xsID_stdout)) {
// 		xsNewHostProperty(xsVar(0), xsID_stdout, xsResult, xsDefault, xsDontDelete | xsDontEnum | xsDontSet);
// 		NSPipe *pipe = [NSPipe pipe];
// 		[task setStandardOutput:pipe];
// 		stdoutFile = [pipe fileHandleForReading];
// 	}
// 	else {
// 		xsNewHostProperty(xsVar(0), xsID_stdout, xsNull, xsDefault, xsDontDelete | xsDontEnum | xsDontSet);
// 	}
// 	
	exec->usage++;
// 	[task launch];

	// if (stderrFile) {
// 		stderrFile.readabilityHandler = ^(NSFileHandle *file) {
// 			NSData *data = [file readDataToEndOfFile];
// 			const char *string = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] UTF8String];
// 			exec->usage++;
// 			FskThreadPostCallback(KprShellGetThread(gShell), (FskThreadCallback)KPR_shell_execute_stderr, exec, FskStrDoCopy(string), NULL, NULL);
// 		};
// 	};
// 	if (stdoutFile) {
// 		stdoutFile.readabilityHandler = ^(NSFileHandle *file) {
// 			NSData *data = [file readDataToEndOfFile];
// 			const char *string = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] UTF8String];
// 			exec->usage++;
// 			FskThreadPostCallback(KprShellGetThread(gShell), (FskThreadCallback)KPR_shell_execute_stdout, exec, FskStrDoCopy(string), NULL, NULL);
// 		};
// 	};
// 	task.terminationHandler = ^(NSTask *task) {
// 		if (stderrFile)
// 			stderrFile.readabilityHandler = NULL;
// 		if (stdoutFile)
// 			stdoutFile.readabilityHandler = NULL;
// 		exec->status = task.terminationStatus;
// 		FskThreadPostCallback(KprShellGetThread(gShell), (FskThreadCallback)KPR_shell_execute_callback, exec, NULL, NULL, NULL);
// 	};
// 	
	xsResult = xsVar(0);
}

void KPR_shell_splitError(xsMachine* the)
{
	xsStringValue string, p;
	char c;
	xsIndex ids[4];
	size_t offsets[4], lengths[4];
	int i;
	xsVars(1);
	string = p = xsToString(xsArg(0));
	c = *p;
	if (c != '/') goto bail;
	ids[0] = xsID_path;
	offsets[0] = p - string;
	p = FskStrChr(p, ':');
	if (!p) goto bail;
	lengths[0] = (p - string) - offsets[0];
	p++;
	ids[1] = xsID_line;
	offsets[1] = p - string;
	while (((c = *p)) && ('0' <= c) && (c <= '9'))
		p++;
	if (c != ':') goto bail;
	lengths[1] = (p - string) - offsets[1];
	p++;
	c  = *p;
	if (('0' <= c) && (c <= '9')) {
		p++;
		while (((c = *p)) && ('0' <= c) && (c <= '9'))
			p++;
		if (c != ':') goto bail;
		p++;
		c  = *p;
	}
	if (c != ' ') goto bail;
	p++;
	ids[2] = xsID_kind;
	offsets[2] = p - string;
	p = FskStrChr(p, ':');
	if (!p) goto bail;
	lengths[2] = (p - string) - offsets[2];
	p++;
	c = *p;
	if (c != ' ') goto bail;
	p++;
	ids[3] = xsID_reason;
	offsets[3] = p - string;
	lengths[3] = FskStrLen(p);
	xsResult = xsNewInstanceOf(xsObjectPrototype);
	for (i = 0; i < 4; i++) {
		xsVar(0) = xsStringBuffer(NULL, lengths[i]);
		xsStringValue pp = xsToString(xsVar(0));
		FskMemCopy(pp, pp + offsets[i], lengths[i]);
		xsNewHostProperty(xsResult, ids[i], xsVar(0), xsDefault, xsDontScript);
	}
bail:
	return;
}
