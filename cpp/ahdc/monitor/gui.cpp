/**************************************
 * GUI for ALERT monitoring
 * 
 * @author ftouchte
 * @date Jan 14, 2025
 * **********************************/

#include "gui.h"
#include "fAxis.h"
#include "ahdcExtractor.h"
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <functional>
#include <cstdio>
#include <cstdlib>

#include "TString.h"
#include "TCanvas.h"

/** Constructor */
Window::Window() :
	// Initialisation // Take of the order of declaration
	VBox_main(Gtk::Orientation::VERTICAL),
	VBox_header(Gtk::Orientation::VERTICAL,10),
	VBox_body(Gtk::Orientation::VERTICAL,10),
	VBox_footer(Gtk::Orientation::VERTICAL,10),
	HBox_eventViewer(Gtk::Orientation::HORIZONTAL,10),
	HBox_histograms(Gtk::Orientation::HORIZONTAL,10),
	HBox_footer(Gtk::Orientation::HORIZONTAL,10),
	HBox_info(Gtk::Orientation::HORIZONTAL,15)
	/*HBox_prev(Gtk::Orientation::HORIZONTAL,10),
	HBox_next(Gtk::Orientation::HORIZONTAL,10),
	HBox_pause(Gtk::Orientation::HORIZONTAL,10),
	HBox_run(Gtk::Orientation::HORIZONTAL,10),
	HBox_hipo4(Gtk::Orientation::HORIZONTAL,10),
	HBox_reset(Gtk::Orientation::HORIZONTAL,10)*/
{
	ahdc = new AhdcDetector();
	set_title("ALERT monitoring");
	set_default_size(1378,654);
	set_child(VBox_main);
	
	/********************
	 * HEADER
	 * *****************/
	VBox_main.append(VBox_header);
	VBox_header.append(*Gtk::make_managed<Gtk::Label>("Header") );
	/*******************
	 * BODY
	 * ****************/
	VBox_main.append(VBox_body);
	VBox_body.append(Book);
	Book.set_margin(10);
	Book.set_expand(true);
	Book.signal_switch_page().connect(sigc::mem_fun(*this,&Window::on_book_switch_page) );
	
	// Page 0
	Book.append_page(HBox_eventViewer, "Event Viewer");
	HBox_eventViewer.append(Grid_eventViewer);
	Grid_eventViewer.set_column_homogeneous(true);
	Grid_eventViewer.set_row_homogeneous(true);
	//Picture_event.set_filename("./ressource/no_event.png");
	//Grid_eventViewer.attach(Picture_event,1,1);
	Grid_eventViewer.attach(DrawingArea_event,1,1);
	DrawingArea_event.set_draw_func(sigc::mem_fun(*this, &Window::on_draw_event) );
	Grid_eventViewer.attach(Grid_waveforms,2,1);
	Grid_waveforms.set_expand(true);
	Grid_waveforms.set_column_homogeneous(true);
	Grid_waveforms.set_row_homogeneous(true);
	// Page 1
	Book.append_page(HBox_histograms, "Histograms");
	HBox_histograms.append(*Gtk::make_managed<Gtk::Label>("Missing histograms ! Add a widget here !") );
	// Page 2 (test)
	Book.append_page(DrawingArea_test, "Test");
	DrawingArea_test.set_draw_func(sigc::mem_fun(*this, &Window::on_draw_test) );

	/******************
	 * FOOTER
	 * ***************/
	VBox_footer.append(HBox_footer);
	HBox_footer.set_margin(10);
	// prev
	HBox_footer.append(Button_prev);
	//Button_prev.set_hexpand(true);
	auto HBox_prev = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL,15);
	Button_prev.set_child(*HBox_prev);
	img_prev.set("./img/icon_prev_off.png");
	HBox_prev->append(img_prev );
	HBox_prev->append(*Gtk::make_managed<Gtk::Label>("prev") );
	Button_prev.signal_clicked().connect( sigc::mem_fun(*this, &Window::on_button_prev_clicked) );

	// next
	HBox_footer.append(Button_next);
	auto HBox_next = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL,15);
	Button_next.set_child(*HBox_next);
	img_next.set("./img/icon_next_off.png");
	HBox_next->append(img_next);
	HBox_next->append(*Gtk::make_managed<Gtk::Label>("next") );
	Button_next.signal_clicked().connect( sigc::mem_fun(*this, &Window::on_button_next_clicked) );

	// pause
	HBox_footer.append(Button_pause);
	auto HBox_pause = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL,20);
	Button_pause.set_child(*HBox_pause);
	img_pause.set("./img/icon_pause_off.png");
	HBox_pause->append(img_pause );
	HBox_pause->append(*Gtk::make_managed<Gtk::Label>("pause") );
	Button_pause.signal_clicked().connect( sigc::mem_fun(*this, &Window::on_button_pause_clicked) );

	// run
	HBox_footer.append(Button_run);
	auto HBox_run = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL,20);
	Button_run.set_child(*HBox_run);
	img_run.set("./img/icon_run_off.png");
	HBox_run->append(img_run);
	HBox_run->append(*Gtk::make_managed<Gtk::Label>("run") );
	Button_run.signal_clicked().connect( sigc::mem_fun(*this, &Window::on_button_run_clicked) );

	// middle
	HBox_footer.append(HBox_info);
	HBox_info.set_hexpand(true);
	HBox_info.append(Label_info);
	Label_info.set_text("No data");

	// hipo4
	HBox_footer.append(Button_hipo4);
	auto HBox_hipo4 = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL,20);
	Button_hipo4.set_child(*HBox_hipo4);
	img_hipo4.set("./img/icon_file_on.png");
	HBox_hipo4->append(img_hipo4);
	HBox_hipo4->append(*Gtk::make_managed<Gtk::Label>("hipo4") );
	Button_hipo4.signal_clicked().connect( sigc::mem_fun(*this, &Window::on_button_hipo4_clicked) );

	// reset
	HBox_footer.append(Button_reset);
	auto HBox_reset = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL,20);
	Button_reset.set_child(*HBox_reset);
	img_reset.set("./img/icon_reset_off.png");
	HBox_reset->append(img_reset);
	HBox_reset->append(*Gtk::make_managed<Gtk::Label>("reset") );
	Button_reset.signal_clicked().connect( sigc::mem_fun(*this, &Window::on_button_reset_clicked) );
	
	// ending
	VBox_main.append(VBox_footer);
	VBox_footer.append(*Gtk::make_managed<Gtk::Label>("Footer") );
}

/** Destructor */
Window::~Window() {
	// nothing
	delete ahdc;
}

void Window::on_button_prev_clicked(){
	std::cout << "Process prev event ..." << std::endl;
}

void Window::on_button_next_clicked(){
	std::cout << "Process next event ..." << std::endl;
	if (filename.size() == 0) {
		return;
	}
	if (hipo_nEvent == 0) {
		hipo_reader.open(filename.c_str());
		hipo_banklist = hipo_reader.getBanks({"AHDC::adc","AHDC::wf:136"});
	}
	this->dataEventAction();
	hipo_nEvent++;
}

void Window::on_button_pause_clicked(){
	std::cout << "Pause ..." << std::endl;
}

void Window::on_button_run_clicked(){
	std::cout << "Run ..." << std::endl;
}

void Window::on_button_hipo4_clicked(){
	std::cout << "Open file dialog ..." << std::endl;

	auto dialog = Gtk::FileDialog::create();

	// Add filters, so that only certain file types can be selected:
	auto filters = Gio::ListStore<Gtk::FileFilter>::create();

	auto filter_hipo4 = Gtk::FileFilter::create();
	filter_hipo4->set_name("Hipo files");
	filter_hipo4->add_pattern("*.hipo");
	filters->append(filter_hipo4);
	
	auto filter_evio = Gtk::FileFilter::create();
	filter_evio->set_name("Evio files");
	filter_evio->add_pattern("*.evio");
	filters->append(filter_evio);
	
	auto filter_text = Gtk::FileFilter::create();
	filter_text->set_name("Text files");
	filter_text->add_mime_type("text/plain");
	filters->append(filter_text);

	auto filter_cpp = Gtk::FileFilter::create();
	filter_cpp->set_name("C/C++ files");
	filter_cpp->add_mime_type("text/x-c");
	filter_cpp->add_mime_type("text/x-c++");
	filter_cpp->add_mime_type("text/x-c-header");
	filters->append(filter_cpp);

	auto filter_any = Gtk::FileFilter::create();
	filter_any->set_name("Any files");
	filter_any->add_pattern("*");
	filters->append(filter_any);

	dialog->set_filters(filters);

	// Show the dialog and wait for a user response:
	dialog->open(sigc::bind(sigc::mem_fun(*this, &Window::on_file_dialog_finish), dialog));
}
void Window::on_file_dialog_finish(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog) {
	// Handle the response:
	try
	{
		auto file = dialog->open_finish(result);

		// Notice that this is a std::string, not a Glib::ustring.
		filename = file->get_path();
		std::cout << "File selected : " <<  filename << std::endl;
		
		// Possible action
		img_next.set("./img/icon_next_on.png"); img_next.queue_draw();
		img_run.set("./img/icon_run_on.png"); img_run.queue_draw();
		img_hipo4.set("./img/icon_file_off.png"); img_hipo4.queue_draw();
	}
	catch (const Gtk::DialogError& err)
	{
		// Can be thrown by dialog->open_finish(result).
		std::cout << "No file selected. " << err.what() << std::endl;
	}
	catch (const Glib::Error& err)
	{
		std::cout << "Unexpected exception. " << err.what() << std::endl;
	}
}

void Window::on_button_reset_clicked(){
	std::cout << "Reset ..." << std::endl;
	hipo_nEvent = 0;
	filename = "";
	img_next.set("./img/icon_next_off.png"); img_next.queue_draw();
	img_run.set("./img/icon_run_off.png"); img_run.queue_draw();
	img_hipo4.set("./img/icon_file_on.png"); img_hipo4.queue_draw();
}

void Window::on_book_switch_page(Gtk::Widget * _pages, guint page_num) { 
	std::string page_name;
	switch (page_num) {
		case 0 :
			page_name = "Event Viewer";
			break;
		case 1 :
			page_name = "Histograms";
			break;
		default :
			page_name = "Unknown process";
	}
	std::cout << "Switch to " << page_name << " tab ..." << std::endl;
}

void Window::on_draw_event(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height){
	cr->save();
	
	// Set relative coordinates
	cr->translate(width/2.0, height/2.0);
	cr->scale(width / 2.0, height / 2.0);
	// Draw the axis
	cr->set_line_width(LINE_SIZE);
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->rectangle(-1.0,-1.0,2.0,2.0);
	cr->stroke();
	// Draw AHDC geometry	
	for (int s = 0; s < ahdc->GetNumberOfSectors(); s++) {
		for (int sl = 0; sl < ahdc->GetSector(s)->GetNumberOfSuperLayers(); sl++){
			for (int l = 0; l < ahdc->GetSector(s)->GetSuperLayer(sl)->GetNumberOfLayers(); l++){
				for (int w = 0; w < ahdc->GetSector(s)->GetSuperLayer(sl)->GetLayer(l)->GetNumberOfWires(); w++){
					AhdcWire wire = *ahdc->GetSector(s)->GetSuperLayer(sl)->GetLayer(l)->GetWire(w);
					// max radius == 68 cm
					// renormalise the coordinates
					double xc, yc;
					this->normalise_coords(0.92, 68.0, 68.0, wire.top.x, wire.top.y, xc, yc);
					cr->set_line_width(LINE_SIZE);
					cr->set_source_rgba(0.0, 0.0, 0.0,0.5);
					cr->arc(xc,yc,WIRE_SIZE,0,2*M_PI);
					cr->stroke();
				}
			}
		}
	}
	// Show activated wires
	for (AhdcWire wire : ListOfWires) {
		double xc, yc;
		this->normalise_coords(0.92, 68.0, 68.0, wire.top.x, wire.top.y, xc, yc);
		//cr->set_line_width(LINE_SIZE);
		cr->set_source_rgb(1.0, 0.0, 0.0);
		cr->arc(xc,yc,WIRE_SIZE,0,2*M_PI);
		cr->fill();
	}

	cr->restore();
}

void Window::normalise_coords(double scale, double x_max, double y_max, double x_old, double y_old, double & x_new, double & y_new){
	x_new = scale*x_old/x_max;
	y_new = scale*y_old/y_max;
}

void Window::on_draw_test(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height){
	cr->save();
	
	//std::vector<double> vx = {1,2,3,4,5};
	//std::vector<double> vy = {1,4,9,16,25};
	std::vector<double> vx, vy;
	for (int i = 0; i<= 2000; i++) {
		double x = i*10/2000.0;
		vx.push_back(x);
		vy.push_back(x+cos(x));
	}

	cairo_plot_graph(cr,width,height,vx,vy);

	cr->restore();
}

void Window::cairo_plot_graph(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height, std::vector<double> vx, std::vector<double> vy){
	int window_size = std::min(width,height);
	// Setting parameters
	double plot_ratio = 0.10; // 10%
	int top_margin = plot_ratio*window_size; 
	int bottom_margin = plot_ratio*window_size;
	int left_margin = plot_ratio*window_size;
	int right_margin = plot_ratio*window_size;
	// Effective window size
	int weff = width - left_margin - right_margin;
	int heff = height - top_margin - bottom_margin;
	int seff = std::min(weff, heff);
	printf("weff : %d , heff : %d\n",weff, heff);
	// Change the origin to be 
	cr->translate(left_margin, top_margin + heff);
	// Determine the min and the max of the data	
	int Npts = vx.size();
	if (Npts != (int) vy.size()) {return ;}
	double xmin = vx[0], xmax = vx[0];
	double ymin = vy[0], ymax = vy[0];
	for (int i = 0; i < Npts; i++){
		xmin = (xmin < vx[i]) ? xmin : vx[i];
 		xmax = (xmax > vx[i]) ? xmax : vx[i];
		ymin = (ymin < vy[i]) ? ymin : vy[i];
		ymax = (ymax > vy[i]) ? ymax : vy[i];
	}
	double margin_ratio = 0.05;
	double x_start = (xmin == 0) ? 0 : xmin - margin_ratio*(xmax - xmin);
	double x_end = xmax + margin_ratio*(xmax - xmin);
	double y_start = (ymin == 0) ? 0 : ymin - margin_ratio*(ymax - ymin);
	double y_end = ymax + margin_ratio*(ymax - ymin);
	printf("x_min   : %lf, x_max : %lf, y_min   : %lf, y_max : %lf\n", xmin, xmax, ymin, ymax);
	printf("x_start : %lf, x_end : %lf, y_start : %lf, y_end : %lf\n", x_start, x_end, y_start, y_end);
	// Define scale functions
	// transform [x1,x2] to [y1,y2]
	// return the y correspnding to a x
	auto linear_transformation = [](double x1, double y1, double x2, double y2, double x) -> double {
		if (x1 == x2) {return x;} // do nothing
		double slope = (y2-y1)/(x2-x1);
		double y=  slope*(x-x1) + y1;
		return y;
	};
	// x coord to width
	auto x2w = [x_start, x_end, weff, linear_transformation] (double x) {
		return linear_transformation(x_start, 0, x_end, weff, x);
	};
	// y coord to height
	auto y2h = [y_start, y_end, heff, linear_transformation] (double y) {
		return linear_transformation(y_start, 0, y_end, -heff, y); // minus heff because of the axis orientation
	};
	
	// Draw points
	for (int i = 0; i < Npts; i++) {
		// represent a marker as a filled circle
		//cr->set_source_rgb(1.0, 0.0, 0.0);
		//cr->arc(vx[i],-vy[i],0.005,0,2*M_PI); // the y is inversed
		//cr->fill();
		// draw a line between points i and i+1
		if (i > 0) {
			cr->set_source_rgb(0.0, 0.0, 1.0);
			cr->set_line_width(0.01*seff);
			cr->move_to(x2w(vx[i-1]),y2h(vy[i-1]));
			cr->line_to(x2w(vx[i]),y2h(vy[i]));
			cr->stroke();
		}
	}

	// Draw the frame for axis
	cr->set_source_rgb(0.0, 0.0, 0.0);
	cr->set_line_width(0.01*seff);
	cr->rectangle(0,0,weff,-heff);
	cr->stroke();
	
	// Define axis
	int n2x = 5; // number of secondary division in the x axis
	int n2y = (int) (5.0*heff/weff); // number of secondary division in the y axis		
	//int n2y = n2x;
	int stick_size1 = 0.025*seff;
	int stick_size2 = 0.020*seff;
	int xlabel_size = 0.4*bottom_margin;
	int ylabel_size = 0.4*left_margin; 
	fAxis ax(x_start, x_end, n2x, 0);
	fAxis ay(y_start, y_end, n2y, 0);
	ax.print();
	ay.print();
	// Draw main sticks x
	for (std::string s : ax.get_labels1()) {
		double value = std::atof(s.c_str());
		if ((value >= x_start) && (value <= x_end)) {
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->set_line_width(0.01*seff);
			cr->move_to(x2w(value), 0);
			cr->line_to(x2w(value), -stick_size1);
			cr->stroke();
			// draw label
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->select_font_face("@cairo:sans-serif",Cairo::ToyFontFace::Slant::NORMAL,Cairo::ToyFontFace::Weight::NORMAL);
			cr->set_font_size(xlabel_size);
			cr->move_to(x2w(value), bottom_margin*0.6);
			cr->show_text(s.c_str());
		}
	}
	// Draw main sticks y
	for (std::string s : ay.get_labels1()) {
		double value = std::atof(s.c_str());
		if ((value >= y_start) && (value <= y_end)) {
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->set_line_width(0.01*seff);
			cr->move_to(0, y2h(value));
			cr->line_to(stick_size1, y2h(value));
			cr->stroke();
			// draw label
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->select_font_face("@cairo:sans-serif",Cairo::ToyFontFace::Slant::NORMAL,Cairo::ToyFontFace::Weight::NORMAL);
			cr->set_font_size(ylabel_size);
			cr->move_to(-left_margin*0.9, y2h(value));
			cr->show_text(s.c_str());
		}
	}
	// Draw secondary sticks x
	for (std::string s : ax.get_labels2()) {
		double value = std::atof(s.c_str());
		if ((value >= x_start) && (value <= x_end)) {
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->set_line_width(0.01*seff);
			cr->move_to(x2w(value), 0);
			cr->line_to(x2w(value), -stick_size2);
			cr->stroke();
			// draw label
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->select_font_face("@cairo:sans-serif",Cairo::ToyFontFace::Slant::NORMAL,Cairo::ToyFontFace::Weight::NORMAL);
			cr->set_font_size(xlabel_size);
			cr->move_to(x2w(value), bottom_margin*0.6);
			cr->show_text(s.c_str());
		}
	}
	// Draw seconday sticks y
	for (std::string s : ay.get_labels2()) {
		double value = std::atof(s.c_str());
		if ((value >= y_start) && (value <= y_end)) {
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->set_line_width(0.01*seff);
			cr->move_to(0, y2h(value));
			cr->line_to(stick_size2, y2h(value));
			cr->stroke();
			// draw label
			cr->set_source_rgb(0.0, 0.0, 0.0);
			cr->select_font_face("@cairo:sans-serif",Cairo::ToyFontFace::Slant::NORMAL,Cairo::ToyFontFace::Weight::NORMAL);
			cr->set_font_size(ylabel_size);
			cr->move_to(-left_margin*0.9, y2h(value));
			cr->show_text(s.c_str());
		}
	}
}

void Window::dataEventAction() {
	// hipo4
	if (hipo_reader.next(hipo_banklist)) {
		// loop over hits
		ListOfWires.clear();
		ListOfSamples.clear();
		for (int col = 0; col < hipo_banklist[1].getRows(); col++){
			int sector = hipo_banklist[1].getInt("sector",col);	
			int layer = hipo_banklist[1].getInt("layer",col);
			int component = hipo_banklist[1].getInt("component",col);
			std::vector<short> samples;
			for (int bin=0; bin < 136; bin++){
				std::string binName = "s" + std::__cxx11::to_string(bin+1);
				short value = hipo_banklist[1].getInt(binName.c_str(),col);
				samples.push_back(value);
			}
			ahdcExtractor T(44,0.5f,5,0.3f);
			T.adcOffset = (short) (samples[0] + samples[1] + samples[2] + samples[3] + samples[4])/5;
			std::map<std::string,double> output = T.extract(samples);
			//T.Show(TString::Format("./ressource/wf%d.png",col+1));
			ListOfWires.push_back(*ahdc->GetSector(sector-1)->GetSuperLayer((layer/10)-1)->GetLayer((layer%10)-1)->GetWire(component-1));
			ListOfSamples.push_back(samples);
		}
		
		// Clean Grid_waveforms
		if (hipo_nEvent != 0) {
			Grid_waveforms.remove_column(2);
			Grid_waveforms.remove_column(1);
		}
		nWF = hipo_banklist[1].getRows();
		std::cout << "nWF : " << nWF << std::endl;
		// Fill Grid_waveforms
		for (int row = 1; row <= (nWF/2); row++) {
			std::vector<double> vx, vy1, vy2;
			for (int i = 0; i < (int) ListOfSamples[0].size(); i++) {
				vx.push_back(i);
				vy1.push_back(ListOfSamples[2*row-1-1][i]);
				vy2.push_back(ListOfSamples[2*row-1][i]);
			}
			// column 1
			auto area1 = Gtk::make_managed<Gtk::DrawingArea>();
			area1->set_draw_func([this, vx,  vy1] (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
								cairo_plot_graph(cr,width,height,vx,vy1);
					 	      } );
			Grid_waveforms.attach(*area1,1,row);
			// column 2
			auto area2 = Gtk::make_managed<Gtk::DrawingArea>();
			area2->set_draw_func([this, vx,  vy2] (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
								cairo_plot_graph(cr,width,height,vx,vy2);
					 	      } );
			Grid_waveforms.attach(*area2,2,row);
		}
		if (nWF % 2 == 1) {
			std::vector<double> vx, vy1;
			for (int i = 0; i < (int) ListOfSamples[0].size(); i++) {
				vx.push_back(i);
				vy1.push_back(ListOfSamples[nWF-1][i]); // the last waveform in that case
			}
			auto area1 = Gtk::make_managed<Gtk::DrawingArea>();
			area1->set_draw_func([this, vx,  vy1] (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
								cairo_plot_graph(cr,width,height,vx,vy1);
					 	      } );
			Grid_waveforms.attach(*area1,1,nWF);
		}
	}
	// Event info
	DrawingArea_event.queue_draw();
	Grid_waveforms.queue_draw();
	Label_info.set_text(TString::Format("Event number : %lu  , Number of WF : %d ...",hipo_nEvent, nWF).Data() );
}

/** Main function */
int main (int argc, char * argv[]) {
	std::cout << "Start GUi..." << std::endl;

	auto app = Gtk::Application::create("org.gtkmm.example");

	return app->make_window_and_run<Window>(argc, argv);
}



