#pragma once

#define MEMORY_STEPS 10
#define MEMORT_TEXT_SIZE 256

class UndoRedo
{
public:
	UndoRedo()
	{
		storeage_thread = NULL;
		storage_variable = NULL;
		storage_text = NULL;
	}

	HWND hWndTB4;

	int CurentPosition;
	int MaximalPosition;
	//What we would like to store
	Thread_t *ptr_thread;
	Variable_t *ptr_variable;


	Thread_t *storeage_thread;
	Variable_t *storage_variable;
	WCHAR **storage_text;

	//Initialise with things we would like to store for redo/undo purpose
	UndoRedo(Thread_t *thread_ext, Variable_t *variable_ext){

		//Copy pointers to save writing
		ptr_thread = thread_ext;
		ptr_variable = variable_ext;

		storeage_thread = new Thread_t[MEMORY_STEPS+1];
		storage_variable = new Variable_t[MEMORY_STEPS+1];
		storage_text = new WCHAR*[MEMORY_STEPS + 1];

		for (int i = 0; i <MEMORY_STEPS; i++)
		{
			storage_text[i] = new WCHAR[MEMORT_TEXT_SIZE];
		}

		CurentPosition = -1;
		MaximalPosition = -1;
	}
	 ~UndoRedo(){

		 delete[] storeage_thread;
		 delete[] storage_variable;

		 for (int i = 0; i <MEMORY_STEPS; i++)
		 {
			 if (storage_text[i])  delete[] storage_text[i];
		 }
		 if (storage_text) delete[]storage_text;
	}

	void AddNewRecord(WCHAR* add_text);
	void Undo();
	void Redo();
	void Initialize();
	void Tooltip(LPARAM lParam);

private:
	void Buttons();

};

