// OVIMSpaceChewing: Chewing Module for OpenVanilla

// Originally written by Kang-min Liu, 2004-2005
// Maintained by zonble, 2005-2006
// Refactored by lukhnos, 2007

// This package is released under the Artistic License,
// please refer to LICENSE.txt for the terms of use and distribution

#define OV_DEBUG

#include <OpenVanilla/OpenVanilla.h>
#include <OpenVanilla/OVLibrary.h>
#include <OpenVanilla/OVUtility.h>

#include <sys/stat.h>
#include <string>
#include <stdlib.h>

#include "chewing.h"
#include "chewingio.h"

#ifdef LIBCHEWING_VERSION_OSX_INCLUDE
	#include "libchewing_version.h"
	#include <Carbon/Carbon.h>
#endif

#ifndef LIBCHEWING_VERSION
    #define LIBCHEWING_VERSION  "(trunk)"
#endif

using namespace std;

static void verbose_logger(void *data, int level, const char *fmt, ...)
{
	// Remove this line for debugging
	return;

	va_list ap;
	FILE *fp = fopen("/tmp/OVIMSpaceChewing.log", "a");

	va_start(ap, fmt);
	vfprintf(fp, fmt, ap);
	va_end(ap);
	fprintf(fp, "\r");
	fclose(fp);
}


bool ChewingFileExist(const char *path, const char *file, const char *sep="/") {
    string fn;
    fn = path;
    fn += sep;
    fn += file;

    struct stat st;
    if (stat(fn.c_str(), &st)) return false;
    return true;
}

bool ChewingCheckData(const char *path) {
    const char *files[]={
        "dictionary.dat",
        "index_tree.dat",
        "pinyin.tab",
        "swkb.dat",
        "symbols.dat",
        NULL,
    };
    
    for (int i=0; files[i]; i++) if (!ChewingFileExist(path, files[i])) return false;
    return true;
}

int utf8_seek(const char *s, int pos) {
	int slen = strlen(s);
	int spos = 0;
	for (int i = 0; i < pos && spos < slen; i++) {
			unsigned char c = s[spos];
			int char_len;
			if (c < 0x80) {
					char_len = 1;
			} else if (c < 0xc0) {
					break; // invalid
			} else if (c < 0xe0) {
					char_len = 2;
			} else if (c < 0xf0) {
					char_len = 3;
			} else if (c < 0xf8) {
					char_len = 4;
			} else {
					break; // invalid
			}
			spos += char_len;
	}
	if (spos > slen)
			spos = slen;
	return spos;
}

class OVIMSpaceChewing;

class OVIMSpaceChewingContext : public OVInputMethodContext 
{
public:
    OVIMSpaceChewingContext(ChewingContext *chewingContext) : im(chewingContext) {}
	
    virtual void start(OVBuffer *key, OVCandidate *textbar, OVService *srv)
	{
	}
	
    virtual void clear()
	{
		chewing_handle_Enter(im);
	}
	
    virtual void end()
	{
		chewing_handle_Enter(im);
	}
     
    virtual int keyEvent(OVKeyCode *key, OVBuffer *buf, OVCandidate *textbar, OVService *srv)
	{
		// fix the SHIFT-SPACE problem
		if (!textbar->onScreen() && buf->isEmpty() && key->isShift() && key->code() == 32) {
			buf->append("　")->send();
			return 1;
		}

        // see if it's the combination of CTRL-OPT-x
        if (!(key->isOpt() && key->isCtrl())) {
            if(key->isCommand()) return 0;
            if(!isprint(key->code()) && buf->isEmpty()) return 0;
        }
		
        KeyPress(key,buf,textbar,srv);
        if(chewing_keystroke_CheckIgnore(im)) return 0;
        
		CandidateWindow(textbar, srv);
        Redraw(buf, srv);
		Notify(srv);
	
        return 1;
    }
  
protected:
    void KeyPress(OVKeyCode *key, OVBuffer *buf, OVCandidate *textbar, OVService *srv)
	{
		int k = key->code();
		Capslock(key,buf,textbar,srv);
		if(k == ovkSpace) {
			key->isShift() ? chewing_handle_ShiftSpace(im):chewing_handle_Space(im);
        }
		else if (k == ovkLeft)   {
		   key->isShift() ? chewing_handle_ShiftLeft(im):chewing_handle_Left(im);
        }
        else if (k == ovkRight)  {
           key->isShift() ?chewing_handle_ShiftRight(im):chewing_handle_Right(im);
        }
        else if (k == ovkDown) { chewing_handle_Down(im); }
        else if (k == ovkUp)   { chewing_handle_Up(im);   }
        else if (k == ovkPageUp) { chewing_handle_PageUp(im);   }
        else if (k == ovkPageDown) { chewing_handle_PageDown(im);   }
        else if (k == ovkEsc)  { chewing_handle_Esc(im);  }
        else if (k == ovkTab)  { chewing_handle_Tab(im);  }
        else if (k == ovkHome) { chewing_handle_Home(im); }
        else if (k == ovkEnd)  { chewing_handle_End(im); }
        else if (k == ovkDelete) { chewing_handle_Del(im); }
		else if (k == ovkBackspace) { chewing_handle_Backspace(im) ; }
        else if (k == ovkReturn) { chewing_handle_Enter(im); }
        else { 
            DefaultKey(key,buf,textbar,srv);
        }
    }

    void DefaultKey(OVKeyCode *key, OVBuffer *buf, OVCandidate *textbar, OVService *srv) {
        if(key->isCtrl()) {
            if((key->code() >= '0') && (key->code() <= '9')) {
				chewing_handle_CtrlNum( im, (key->code()));
            }
			else if(key->isAlt()) { 
				// chewing_handle_CtrlOption is removed from libchewing.
			}
            return;
        }
        chewing_handle_Default(im ,(key->isShift())?toupper(key->code()):tolower(key->code()));
    }
	
    void Capslock(OVKeyCode *key, OVBuffer *buf, OVCandidate *textbar, OVService *srv) {
		if(key->isCapslock()) {
			if(chewing_get_ChiEngMode(im) == CHINESE_MODE) {
				chewing_handle_Capslock(im);
			}
		}
		else if (chewing_get_ChiEngMode(im) != CHINESE_MODE) {
			chewing_handle_Capslock(im);
       }
    }

    void Redraw(OVBuffer *buf, OVService *srv) {
		if ( chewing_commit_Check( im ) ) {
			const char *s = chewing_commit_String( im );
			buf->clear()->append(s)->send();
        }
		
        int ps=-1, pe=-1;
		
	int ips=-1, ipe=0;
		
        if (ips > -1 && ipe !=0) {
            if (ipe > 0) {
				ps=ips;
				pe=ps+ipe;
			}
            else {
				ps=ips+ipe;
				pe=ips;
			}
        }
		
        // murmur("ips=%d, ipe=%d, ps=%d, pe=%d\n", ips, ipe, ps, pe);
		
        const char *s1, *s2;
		int s1len, s2len;
		int pos = chewing_cursor_Current(im);
		buf->clear();
		s1 = chewing_buffer_String(im);
		s1len = s1 ? strlen(s1) : 0;
		s2 = chewing_bopomofo_String_static(im);
		s2len = s2 ? strlen(s2) : 0;
		int slen = s1len + s2len;
		char *s = (char*)calloc(1, slen + 1);
		if (s1) {
				strcpy(s, s1);
				chewing_free((void*)s1);
		}
		int spos = utf8_seek(s, pos);
		memmove(s + spos + s2len, s + spos, s1len - spos);
		memcpy(s + spos, s2, s2len);

		buf->append(s);
		buf->update(pos, ps, pe);
		
        //murmur("==> %s%s%s",s1,s2,s3);
    }

    void Notify(OVService *srv) {
       if(chewing_aux_Length(im) != 0) srv->notify(chewing_aux_String(im));
    }
    
    void CandidateWindow(OVCandidate *textbar, OVService *srv) {
        if(chewing_cand_TotalPage(im) > 0) {
			char s[64];
			int i=1;
			char str[ 20 ];
			char *cand_string;
			
			textbar->clear();
			chewing_cand_Enumerate( im );
			while ( chewing_cand_hasNext( im ) ) {
			   if ( i > chewing_cand_ChoicePerPage( im ) ) break;
			   
			   int *selKey = chewing_get_selKey(im);
			   if (!selKey) break;
			   sprintf( str, "%c.", selKey[i - 1]);
			   chewing_free(selKey);

			   textbar->append( str );
			   cand_string = chewing_cand_String( im );
			   sprintf( str, " %s ", cand_string );
			   textbar->append( str );
			   chewing_free( cand_string );
			   i++;
			}
			sprintf(s," %d/%d", chewing_cand_CurrentPage(im) + 1, chewing_cand_TotalPage(im));
			textbar->append((char*)s)->update()->show();
		}
		else {
		   textbar->hide();
		}
    }

protected:
    OVIMSpaceChewing *parent;
    ChewingContext *im;
};

class OVIMSpaceChewing : public OVInputMethod
{
public:
	OVIMSpaceChewing() : chewingContext(NULL)
	{
	}

	virtual ~OVIMSpaceChewing()
	{
		chewing_delete(chewingContext);
	}

	virtual int initialize(OVDictionary* l, OVService* s, const char* modulePath) {
        string hashdir;
        hashdir = s->userSpacePath(identifier());
        hashdir += s->pathSeparator();
        hashdir += "uhash.dat";
        
        string chewingpath;
		
		#ifdef LIBCHEWING_VERSION_OSX_INCLUDE
			CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("org.openvanilla.module.ovimspacechewing"));
			if (!bundle) return 0;
			
			CFURLRef url = CFBundleCopyResourcesDirectoryURL(bundle);
			if (!url) return 0;
			
			char buf[1024];
			CFURLGetFileSystemRepresentation(url, TRUE, (UInt8*)buf, sizeof(buf)-1);
			chewingpath = buf;
		#else
			chewingpath = modulePath;
		#endif
        
		if (!ChewingCheckData(chewingpath.c_str())) {
			murmur("OVIMSpaceChewing: chewing data missing at %s", chewingpath.c_str());
			return 0;
		}

		murmur ("OVIMSpaceChewing: Chewing data=%s, userhash=%s",
		  chewingpath.c_str(), hashdir.c_str());

		chewingContext = chewing_new2(chewingpath.c_str(), hashdir.c_str(), verbose_logger, NULL);

        // set default layout to Standard
        int kb = l->getIntegerWithDefault("keyboardLayout", 0);        
		chewing_set_KBType(chewingContext, kb);
		
		const char *selKey_define = "1234567890";
		
		// XXX magic number
		if (kb==1 /* KB_HSU */) selKey_define = "asdfghjkl0";
		if (kb==7 /* KB_DVORAK_HSU */) selKey_define = "aoeuhtn7890";
		
		int *selKey = (int*)calloc(MAX_SELKEY, sizeof(int));
		for (int i=0; i<MAX_SELKEY && selKey_define[i]; i++) selKey[i] = selKey_define[i];
		chewing_set_selKey(chewingContext, selKey, strlen(selKey_define));
		free(selKey);
		

		// Enable this, because this is called... SpaceChewing
		chewing_set_spaceAsSelection(chewingContext, 1);

		chewing_set_candPerPage(chewingContext, l->getIntegerWithDefault("candPerPage", 7));
		chewing_set_maxChiSymbolLen(chewingContext, 20);
		int dir = l->getIntegerWithDefault("addPhraseForward", 0);
		chewing_set_addPhraseDirection(chewingContext, dir);

		return 1;
	}

    virtual void update(OVDictionary* localconfig, OVService*)
	{
		chewing_set_KBType(chewingContext, localconfig->getInteger("keyboardLayout"));
    }

    virtual const char *identifier()
	{
        return "OVIMSpaceChewing";
    }

    virtual const char *localizedName(const char *locale)
	{
        if (!strcasecmp(locale, "zh_TW")) return "酷音";
        if (!strcasecmp(locale, "zh_CN")) return "繁体酷音";
 	    return "Chewing (Smart Phonetics)";
    }

    virtual OVInputMethodContext* newContext()
	{
	    return new OVIMSpaceChewingContext(chewingContext);
    }
    
protected:
    ChewingContext *chewingContext;
};

// the module wrapper
OV_SINGLE_MODULE_WRAPPER(OVIMSpaceChewing);
