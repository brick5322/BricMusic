#pragma once
#include <QtCore/qglobal.h>

namespace ProcProto
{
	struct Header {
		union
		{
			qint64 r_pid;
			struct {
				qint32 is_valid_pid;
				qint32 code;
			}s_code;
		}identifier;
		char data[];

		char* get_buffer(){
			return identifier.s_code.is_valid_pid == -1 ? 0 : data;
		}

		qint64 get_pid(){
			return identifier.r_pid;
		}

		void set_pid(qint64 pid) {
			identifier.r_pid = pid;
		}

		int get_code(){
			return identifier.s_code.is_valid_pid != -1 ? 0 : identifier.s_code.code;
		}

		void set_code(int code) {
			identifier.s_code.is_valid_pid = -1;
			identifier.s_code.code = code;

		}
	};

};

