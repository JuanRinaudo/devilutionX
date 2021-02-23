#ifndef __JUANEDITOR_H__
#define __JUANEDITOR_H__

DEVILUTION_BEGIN_NAMESPACE

#ifdef __cplusplus
extern "C" {
#endif

extern BOOL juaneditorflag;

void InitJuanEditor();
void DrawJuanEditor();
void CheckJuanEditorBtns();
BOOL PressKeysJuanEditor(int vkey);
BOOL PressCharJuanEditor(int vkey);

#ifdef __cplusplus
}
#endif

DEVILUTION_END_NAMESPACE

#endif /* __JUANEDITOR_H__ */
