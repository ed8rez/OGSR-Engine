////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder.cpp
//	Created 	: 26.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script objects binder
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "ai_space.h"
#include "script_engine.h"
#include "script_binder.h"
#include "xrServer_Objects_ALife.h"
#include "script_binder_object.h"
#include "script_game_object.h"
#include "gameobject.h"
#include "level.h"

CScriptBinder::CScriptBinder		()
{
	init					();
}

CScriptBinder::~CScriptBinder		()
{
	VERIFY					(!m_object);
}

void CScriptBinder::init			()
{
	m_object				= 0;
}

void CScriptBinder::clear			()
{
	try {
		xr_delete			(m_object);
	}
	catch(...) {
		m_object			= 0;
	}
	init					();
}

void CScriptBinder::reinit			()
{
#ifdef DEBUG_MEMORY_MANAGER
	u32									start = 0;
	if (g_bMEMO)
		start							= Memory.mem_usage();
#endif // DEBUG_MEMORY_MANAGER
	if (m_object) {
		__try
		{
			m_object->reinit();
		}
		__except (ExceptStackTrace("[CScriptBinder::reinit] stack_trace:\n"))
		{
			clear();
		}
	}
}

void CScriptBinder::Load			(LPCSTR section)
{
}

void CScriptBinder::reload			(LPCSTR section)
{
#ifdef DEBUG_MEMORY_MANAGER
	u32									start = 0;
	if (g_bMEMO)
		start							= Memory.mem_usage();
#endif // DEBUG_MEMORY_MANAGER
	VERIFY					(!m_object);

	if (!pSettings->line_exist(section,"script_binding"))
		return;

	auto script_func_name = pSettings->r_string(section, "script_binding");
	luabind::functor<void> lua_function;
	if (!ai().script_engine().functor(script_func_name, lua_function)) {
		Msg("!![CScriptBinder::reload] function [%s] not loaded!", script_func_name);
		return;
	}
	
	auto game_object = smart_cast<CGameObject*>(this);
	if (!game_object) //������� ��� - ������ ��� ������ ������.
	{
		Msg("!![[CScriptBinder::reload] failed cast to CGameObject!");
		return;
	}

	__try
	{
		lua_function(game_object->lua_game_object());
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		Msg("!![CScriptBinder::reload] Error in call lua function [%s]", script_func_name);
		clear();
		return;
	}

	if (m_object) {
		__try
		{
			m_object->reload(section);
		}
		__except (ExceptStackTrace("[CScriptBinder::reload] stack_trace:\n"))
		{
			clear();
		}
	}
}

BOOL CScriptBinder::net_Spawn		(CSE_Abstract* DC)
{
#ifdef DEBUG_MEMORY_MANAGER
	u32									start = 0;
	if (g_bMEMO)
		start							= Memory.mem_usage();
#endif // DEBUG_MEMORY_MANAGER
	CSE_Abstract			*abstract = (CSE_Abstract*)DC;
	CSE_ALifeObject			*object = smart_cast<CSE_ALifeObject*>(abstract);
	if (object && m_object) {
		__try
		{
			return (BOOL)m_object->net_Spawn(object);
		}
		__except (ExceptStackTrace("[CScriptBinder::net_Spawn] stack_trace:\n"))
		{
			clear();
		}
	}
	return TRUE;
}

void CScriptBinder::net_Destroy		()
{
	if (m_object) {
#ifdef _DEBUG
		Msg						("* Core object %s is UNbinded from the script object",smart_cast<CGameObject*>(this) ? *smart_cast<CGameObject*>(this)->cName() : "");
#endif // _DEBUG
		__try
		{
			m_object->net_Destroy();
		}
		__except (ExceptStackTrace("[CScriptBinder::net_Destroy] stack_trace:\n"))
		{
			clear();
		}
	}
	xr_delete				(m_object);
}

void CScriptBinder::set_object		(CScriptBinderObject *object)
{
	if (IsGameTypeSingle()) {
		VERIFY2				(!m_object,"Cannot bind to the object twice!");
#ifdef _DEBUG
		Msg					("* Core object %s is binded with the script object",smart_cast<CGameObject*>(this) ? *smart_cast<CGameObject*>(this)->cName() : "");
#endif // _DEBUG
		m_object			= object;
	}
}

void CScriptBinder::shedule_Update	(u32 time_delta)
{
	if (m_object) {
		__try
		{
			m_object->shedule_Update(time_delta);
		}
		__except (ExceptStackTrace("[CScriptBinder::shedule_Update] stack_trace:\n"))
		{
			clear();
		}
	}
}

void CScriptBinder::save			(NET_Packet &output_packet)
{
	if (m_object) {
		__try
		{
			m_object->save(&output_packet);
		}
		__except (ExceptStackTrace("[CScriptBinder::save] stack_trace:\n"))
		{
			clear();
		}
	}
}

void CScriptBinder::load			(IReader &input_packet)
{
	if (m_object) {
		__try
		{
			m_object->load(&input_packet);
		}
		__except (ExceptStackTrace("[CScriptBinder::load] stack_trace:\n"))
		{
			clear();
		}
	}
}

BOOL CScriptBinder::net_SaveRelevant()
{
	if (m_object) {
		__try
		{
			return m_object->net_SaveRelevant();
		}
		__except (ExceptStackTrace("[CScriptBinder::net_SaveRelevant] stack_trace:\n"))
		{
			clear();
		}
	}
	return FALSE;
}

void CScriptBinder::net_Relcase		(CObject *object)
{
	CGameObject						*game_object = smart_cast<CGameObject*>(object);
	if (m_object && game_object) {
		__try
		{
			m_object->net_Relcase(game_object->lua_game_object());
		}
		__except (ExceptStackTrace("[CScriptBinder::net_Relcase] stack_trace:\n"))
		{
			clear();
		}
	}
}
