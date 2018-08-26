#include "MainForm.h"
using namespace Sound2Source;

void MainForm::RealInit()
{
	InitializeComponent();

	PrivateFontCollection^ fonts = gcnew PrivateFontCollection();

	System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
	array<Byte>^ fontData = (cli::safe_cast<array<Byte>^>(resources->GetObject(L"bahnschrift")));
	pin_ptr<System::Byte> p = &fontData[0];
	unsigned char* pby = p;
	char* pch = reinterpret_cast<char*>(pby);

	fonts->AddMemoryFont(IntPtr(pch), fontData->Length);

	auto bahnschriftA = gcnew System::Drawing::Font(fonts->Families[0], 24, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
		static_cast<System::Byte>(0));

	this->label1->Font = bahnschriftA;

	auto bahnschriftB = gcnew System::Drawing::Font(fonts->Families[0], 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
		static_cast<System::Byte>(0));

	this->label2->Font = bahnschriftB;
	this->label3->Font = bahnschriftB;
};

void ProcessUC(String^ file, String^  output) {
	auto bytes = System::IO::File::ReadAllBytes(file);

	for (int i = 0; i < bytes->Length; i++) {
		bytes[i] ^= 0xa3;
	}

	System::IO::File::WriteAllBytes(output, bytes);
}

System::Void MainForm::ProcessAudioList(array<String^>^ files) {
	label1->Text = "Processing";

	if (access("exported_files", 00) == -1) {
		mkdir("exported_files");
	}

	for (int i = 0; i < files->Length; i++)
	{
		auto file = files[i];

		if (file->EndsWith(".ncm")) 
		{
			IntPtr nativeFile = Marshal::StringToHGlobalAnsi(file);
			ProcessNCM((const char *)nativeFile.ToPointer());
		} 
		else if (file->EndsWith(".uc!"))
		{
			auto outputPath = IO::Path::Combine(Environment::CurrentDirectory, "exported_files", IO::Path::GetFileNameWithoutExtension(file)) + ".mp3";
			ProcessUC(file, outputPath);
		}
		else 
		{
			if (comboBox1->Text->Contains("MP3")) {
				auto outputPath = IO::Path::Combine(Environment::CurrentDirectory, "exported_files", IO::Path::GetFileNameWithoutExtension(file)) + ".mp3";
				auto bitRate = Convert::ToInt32(comboBox1->Text->Substring(6, 3));
				bitRate *= 125;
				printf("%d", bitRate);
				ProcessAudio((const WCHAR *)Marshal::StringToHGlobalUni(file).ToPointer(), (const WCHAR *)Marshal::StringToHGlobalUni(outputPath).ToPointer(), bitRate);
			}
			else {
				auto outputPath = IO::Path::Combine(Environment::CurrentDirectory, "exported_files", IO::Path::GetFileNameWithoutExtension(file)) + ".wav";
				ProcessAudio((const WCHAR *)Marshal::StringToHGlobalUni(file).ToPointer(), (const WCHAR *)Marshal::StringToHGlobalUni(outputPath).ToPointer(), 0, 1);
			}
		}

		System::Diagnostics::Process::Start("exported_files");
	}

	label1->Text = "Done!";

	timer = gcnew Timer();
	timer->Interval = 2000;
	timer->Tick += gcnew System::EventHandler(this, &MainForm::Timer_Tick);
	timer->Start();
}

System::Void MainForm::Timer_Tick(System::Object^  sender, System::EventArgs^ e) {
	timer->Stop();
	label1->Text = "Drop here to convert!";
}

System::Void MainForm::label1_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
	if (e->Data->GetDataPresent(DataFormats::FileDrop)) {
		e->Effect = DragDropEffects::Copy;
	}
}

System::Void MainForm::label1_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e) {
	auto files = (array<String^>^)e->Data->GetData(DataFormats::FileDrop);
	ProcessAudioList(files);
}