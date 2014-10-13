#define _WIN32_WINNT 0x0500
#include <ctime>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Output.H>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Struct for getting time info
struct Time
{
	time_t rawtime;
	struct tm timeinfo;
	Time()
	{
		rawtime = time(0);
		localtime_s(&timeinfo, &rawtime);
	}
	void refresh()
	{
		rawtime = time(0);
		localtime_s(&timeinfo, &rawtime);
	}
	string getMonth()
	{
		return to_string(timeinfo.tm_mon + 1);
	}
	string getDay()
	{
		return to_string(timeinfo.tm_mday + 1);
	}
	string getYear()
	{
		return to_string(1900 + timeinfo.tm_year);
	}

};

// Employee Object
class Employee
{
	private:
		string first_name;
		string last_name;
		string id_number;
		bool management;
	public:
		Employee() : first_name(""), last_name(""), id_number(""), management(false){}
		Employee(string fn, string ln, string id, bool man = false)	:
			first_name(fn), last_name(ln), id_number(id), management(man)	{}
		Employee(const Employee& emp)
		{
			first_name = emp.getFirstName();
			last_name = emp.getLastName();
			id_number = emp.getID();
			management = emp.isManagement();
		}
		Employee& operator=(const Employee& emp)
		{
			first_name = emp.getFirstName();
			last_name = emp.getLastName();
			id_number = emp.getID();
			management = emp.isManagement();
			return *this;
		}
		//Getters
		string getFirstName()	const	{return first_name;}
		string getLastName()	const	{return last_name;}
		string getName()	const	{return first_name + " " + last_name;}
		string getDispName() const { return first_name + " " + last_name[0] + "."; }
		string getID()	const	{return id_number;}
		bool isManagement() const {return management;}
		//Setter
		void makeManager()	{management = true;}
		bool operator==(const Employee& em)
		{
			if (first_name == em.first_name && last_name == em.last_name && id_number == em.id_number)
				return true;
			else return false;
		}
};

// Employee Database 
class EmployeeBase
{
	private:
		vector<Employee> emps;
	public:
		void addEmp(string first, string last, string id, bool man)
		{
			emps.push_back(Employee(first, last, id, man));
		}
		EmployeeBase() {}
		EmployeeBase(EmployeeBase& empb)
		{
			for (int i = 0; i < empb.emps_v().size(); ++i)
			{
				emps.push_back(empb.emps_v()[i]);
			}
		}
		void initialize(string fname)
		{
			ifstream ifs(fname.c_str());
			string f, l, id, m;
			bool mm;
			// Read in employee data
			while (ifs >> f >> l >> id >> m)
			{
				// Check management status
				if (m == "0")	mm = false;
				else mm = true;
				addEmp(f, l, id, mm);
			}
		}
		EmployeeBase(string name)
		{
			initialize(name);
		}
		bool isEmp(string id)
		{
			for (int i = 0; i < emps.size(); ++i)
			{
				if (emps[i].getID() == id)
					return true;
			}
			return false;
		}
		vector<Employee> emps_v()
		{
			return emps;
		}
		Employee getEmpByID(string id)
		{
			for (int i = 0; i < emps.size(); ++i)
			{
				if (emps[i].getID() == id)
					return emps[i];
			}
			return Employee("", "", "");
		}
};

// Output operator for Employee object, outputs Employees full name
ostream& operator<<(ostream& out, Employee emp)
{
	out << emp.getDispName();
	return out;
}

struct Item
{
	string name;
	double price;
	string cat;
	bool sent;
	Item(string n, double p, string c) : name(n), price(p), cat(c), sent(false) {}
	bool operator==(const Item& it)
	{
		if (name == it.name && price == it.price && cat == it.cat)
			return true;
		else return false;
	}
};

// Struct defining Category object
struct Category
{
	string name;
	vector<Item> items;
	Category(string n) : name(n) { }
	void addItem(string name, double price, string cat)
	{
		items.push_back(Item(name, price, cat));
	}
	bool operator==(const Category& c)
	{
		if (this->name == c.name)	return true;
		else return false;
	}
};

class CatNotFound{};

// Struct to hold all categories
struct Categories
{
	vector<Category> cats;
	Categories() {}
	Categories(string fname)
	{
		ifstream ifs(fname.c_str());
		string ntem, ctem;
		double tpri;
		while (ifs >> tpri)
		{
			getline(ifs, ntem);
			getline(ifs, ntem);
			getline(ifs, ctem);
			if (!exists(ctem))
				cats.push_back(Category(ctem));
			addItemtoCat(ntem, tpri, ctem);
			getline(ifs, ntem);
		}
	}
	void initialize(string fname)
	{
		ifstream ifs(fname.c_str());
		string ntem, ctem;
		double tpri;
		while (ifs >> tpri)
		{
			getline(ifs, ntem);
			getline(ifs, ntem);
			getline(ifs, ctem);
			if (!exists(ctem))
				cats.push_back(Category(ctem));
			addItemtoCat(ntem, tpri, ctem);
			getline(ifs, ntem);
		}
	}
	Category& findCat(string n)
	{
		for (int i = 0; i < cats.size(); ++i)
		{
			if (cats[i].name == n)	return cats[i];
		}
		throw CatNotFound();
	}
	bool exists(string n)
	{
		for (int i = 0; i < cats.size(); ++i)
		{
			if (cats[i].name == n)	return true;
		}
		return false;
	}
	int size()	{ return cats.size(); }
	void addItemtoCat(string name, double price, string cat)
	{
		try
		{
			findCat(cat).addItem(name, price, cat);
		}
		catch (CatNotFound&)
		{
			cerr << "Category not found!" << endl;
			exit(1);
		}
	}
};

// Struct for Order object
struct Order
{
	int ord_num;
	double total;
	vector<Item> items;
	int tot_quant;
	Employee owner;
	void addItem(const Item& it) { items.push_back(it);	tot_quant = items.size(); total += it.price; }
	void removeItem(int i)
	{
		if (!items[i].sent)
		{
			total -= items[i].price;
			--tot_quant;
			items.erase(items.begin() + i);
			return;
		}
		else
		{
			cerr << "Manager Auth Required to Remove Item";
		}
	}
	Order(int ord, const Employee& emp) : ord_num(ord), owner(emp)	{	}
};

// Struct for managing all Order Objects
struct OrderBase
{
	vector<Order> orders;
	fstream ios;
	string fname;
	Time time;
	OrderBase()
	{
		fname = "Orders/" + time.getMonth() + "-" + time.getDay() + "-" + time.getYear();
		ios.open(fname.c_str(), fstream::in|fstream::out);
	}
	vector<Order> findEmpOrders(const Employee& em)
	{
		vector<Order> spec;
		for (int i = 0; i < orders.size(); ++i)
		{
			if (orders[i].owner == em)
				spec.push_back(orders[i]);
		}
		return spec;
	}
	Order& getOrderByNum(int i)
	{
		for (int i = 0; i < orders.size(); ++i)
		{
			if (orders[i].ord_num == i)
				return orders[i];
		}
	}
};

// Code slightly modified from Erco's FLTK Cheat Sheet
class Fl_Justify_Input : public Fl_Group {
	Fl_Input *inp;
	Fl_Box   *box;
	bool manag;
public:
	friend class NumPad;
	// Ctor
	Fl_Justify_Input(int X, int Y, int W, int H, bool m = false, const char *L = 0) :Fl_Group(X, Y, W, H, L) {
		Fl_Group::box(FL_ROUND_DOWN_BOX);
		manag = m;
		box = new Fl_Box(X, Y, W, H);
		if (m)
		{
			box->labelcolor(FL_RED);
			box->copy_label("Manager Authentication");
			box->labelsize(25);
		}
		box->labelfont(FL_HELVETICA_BOLD);
		box->color(FL_WHITE);
		box->box(FL_ROUND_DOWN_BOX);
		inp = new Fl_Input(X, Y, W, H);
		inp->box(FL_ROUND_DOWN_BOX);
		inp->input_type(5);
		inp->maximum_size(4);
		inp->textsize(40);
		inp->hide();
		inp->color(FL_WHITE);
		end();
	}
	// Set text justification. Expects one of:
	// FL_ALIGN_LEFT, FL_ALIGN_CENTER, FL_ALIGN_RIGHT.
	void justify(Fl_Align val) {
		box->align(val | FL_ALIGN_INSIDE);
	}
	// Returns text justification
	Fl_Align justify() const {
		return box->align();
	}
	// Sets the text value
	void value(const char *val, int num) {
		string temp = "";
		box->labelcolor(FL_BLACK);
		box->labelsize(40);
		for (int i = 0; i < num; ++i) temp += "•";
		box->copy_label(temp.c_str());
		inp->value(val);
	}
	// Gets the text value
	const char *value() const {
		return inp->value();
	}
	void displayWarning()
	{
		box->labelcolor(FL_RED);
		if (!manag){
			box->copy_label("INVALID ID!");
			box->labelsize(40);
		}
		else
		{
			box->copy_label("ACCESS DENIED!");
			box->labelsize(33); 
		}
	}
};

// Authentication Numerical Pad modified Erco's
class NumPad : public Fl_Group
{
	private:
		EmployeeBase empdata;
		Employee currentEmp;
		Fl_Justify_Input *inp;
		Fl_Callback *enter_cb;
		const char *enter_data;
		string daty;
		bool manag;
		bool done;

		//Handle numeric keypad buttons pressed
		void Button_CB2(Fl_Widget *w);
		static void Button_CB(Fl_Widget *w, void *data)
		{
			NumPad *numpad = (NumPad*)data;
			numpad->Button_CB2(w);
		}
	public:
		NumPad(int x, int y, int w, int h, const EmployeeBase& emp, bool m = false, const char *l=0) : Fl_Group(x, y, w, h, l)
		{
			empdata = emp;
			manag = m;
			const int bsize = 75;
			// Preview input
			inp = new Fl_Justify_Input(x+100, y+35, w-200, 50, manag);
			inp->justify(FL_ALIGN_CENTER);
			daty = "";
			// Numeric keypad
			Fl_Button *b;
			int colstart = x+100,
				col = colstart,
				row = inp->y()+inp->h()+10;
			b = new Fl_Button(col, row, bsize, bsize, "1"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			b = new Fl_Button(col, row, bsize, bsize, "2"); b->callback(Button_CB, (void*)this);
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			b = new Fl_Button(col, row, bsize, bsize, "3"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10);  b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			col=colstart; row+=(bsize+10);
			b = new Fl_Button(col, row, bsize, bsize, "4"); b->callback(Button_CB, (void*)this);
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			b = new Fl_Button(col, row, bsize, bsize, "5"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			b = new Fl_Button(col, row, bsize, bsize, "6"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10);  b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			col=colstart; row+=(bsize+10);
			b = new Fl_Button(col, row, bsize, bsize, "7"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10);  b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			b = new Fl_Button(col, row, bsize, bsize, "8"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			b = new Fl_Button(col, row, bsize, bsize, "9"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			col=colstart; row+=(bsize+10);
			b = new Fl_Button(col, row, bsize, bsize, "Clear"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(30);
			b = new Fl_Button(col, row, bsize, bsize, "0"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(40);
			b = new Fl_Button(col, row, bsize, bsize, "Enter"); b->callback(Button_CB, (void*)this); 
			col += (bsize + 10); b->box(FL_PLASTIC_UP_BOX); b->color(fl_rgb_color(77, 26, 77));
			b->labelsize(30);
			end();
			enter_cb = 0;
			enter_data = "";
			done = false;
		}
		// Return value in input
		const char *value()	{ return(inp->value());	}
		// Clear value in input
		void clear() { inp->value("", 0); }
		bool loggedOn()	{ return done; }
		Employee currentUser()	{ return currentEmp; }
		void draw()
		{
			Fl_Widget *const*child_widgets = array();
			if (damage() == FL_DAMAGE_CHILD)
			{
				for (int i = 0; i < children(); ++i)
				{
					update_child(**child_widgets);
					child_widgets++;
				}
			}
			else
			{
				Fl_Color box_color = fl_rgb_color(27, 77, 18);
				if (manag) box_color = fl_rgb_color(255, 0, 0);
				fl_draw_box(FL_ROUND_UP_BOX, x() - 45, y() - 30, w() + 80, h()+20, box_color);
				fl_draw_box(FL_ROUND_DOWN_BOX, x()-25, y() -10, w()+40, h()-20, FL_LIGHT2);
				draw_label();
				for (int j = 0; j < children(); ++j)
				{
					draw_child(**child_widgets);
					draw_outside_label(**child_widgets);
					child_widgets++;
				}
			}
		}
};

// Main Panel in bottom right corner (Manager Functions, Utilities, New Order, Logout buttons)
class ActionPanel : public Fl_Group
{
	private:
		int firstrow;
		int secrow;
		int firstcol;
		int seccol;
		int space;
		int b_w;
		void Button_RCB(Fl_Widget *w);
		static void Button_CB(Fl_Widget *w, void *data)
		{
			ActionPanel *act = (ActionPanel*)data;
			act->Button_RCB(w);
		}
	public:
		ActionPanel(int x, int y, int w, int h, const char* l = 0) : Fl_Group(x, y, w, h, l)
		{  
			space = 5;
			b_w = 100;
			firstrow = y + space;
			firstcol = x + space;
			secrow = firstrow + 80 + space;
			seccol = firstcol + b_w + space;
			box(FL_EMBOSSED_BOX);
			color(fl_rgb_color(194, 173, 123));
			Fl_Button* b;
			b = new Fl_Button(firstcol, firstrow, b_w, 75, "New\nOrder"); b->callback(Button_CB, (void*)this); 
			b->box(FL_PLASTIC_UP_BOX); b->color(FL_BLUE); b->labelsize(20);
			b = new Fl_Button(seccol, firstrow, b_w, 75, "Utilities"); b->callback(Button_CB, (void*)this);
			b->box(FL_PLASTIC_UP_BOX); b->color(FL_BLUE); b->labelsize(20);
			b = new Fl_Button(firstcol, secrow, b_w, 75, "Manager\nFunctions"); b->callback(Button_CB, (void*)this);
			b->box(FL_PLASTIC_UP_BOX); b->color(FL_BLUE); b->labelsize(20);
			b = new Fl_Button(seccol, secrow, b_w, 75, "Logout"); b->callback(Button_CB, (void*)this);
			b->box(FL_PLASTIC_UP_BOX); b->color(FL_BLUE); b->labelsize(20);
			end();
		}
};

const char* month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
const string weekday[7] = { "Sun.", "Mon.", "Tue.",
		"Wed.", "Thu.", "Fri.", "Sat." };

// Creates Blackbox displaying system time information for current day
class TimeBox : public Fl_Group
{
	Fl_Multiline_Output *time_box;
	time_t rawtime;
	struct tm timeinfo;
	string ti;
	string da;

	

	public:
		TimeBox(int x, int y, int w, int h, const char* l = 0) : 
			Fl_Group(x, y, w, h, l)
		{
			rawtime = time(0);
			localtime_s(&timeinfo,&rawtime);
			time_box = new Fl_Multiline_Output(x, y, w, h, l);
			time_box->box(FL_SHADOW_BOX);
			time_box->color(FL_BLACK);
			time_box->textcolor(FL_GREEN);
			time_box->textsize(30);
			time_box->labelfont(FL_COURIER);
			ti = to_string(timeinfo.tm_hour % 12) + ": " + to_string(timeinfo.tm_min);
			if (timeinfo.tm_hour < 12) ti += " am";
			else ti += " pm";
			da = weekday[timeinfo.tm_mday] + " " + month[timeinfo.tm_mon] + " " + to_string(timeinfo.tm_mday) + ", " + to_string(1900 + timeinfo.tm_year);
			time_box->value(string(ti + "\n" + da).c_str());
		}
		void refresh()
		{
			rawtime = time(0);
			localtime_s(&timeinfo, &rawtime);
			time_box->color(FL_BLACK);
			time_box->textcolor(FL_GREEN);
			time_box->labelfont(FL_COURIER);
			ti = to_string(timeinfo.tm_hour % 12) + ":" + to_string(timeinfo.tm_min);
			if (timeinfo.tm_hour < 12) ti += " am";
			else ti += " pm";
			da = weekday[timeinfo.tm_mday] + " " + month[timeinfo.tm_mon] + " " + to_string(timeinfo.tm_mday) + ", " + to_string(1900 + timeinfo.tm_year);
			time_box->value(string("    " + ti + '\n' + da).c_str());
		}
};

class GreetBox : public Fl_Group
{
private:
	Employee emp;
	string what;
	Fl_Multiline_Output* out;
public:
	GreetBox(int x, int y, int w, int h, const char* l = 0) : Fl_Group(x, y, w, h, l)
	{
		out = new Fl_Multiline_Output(x, y, w, h, l);
		out->box(FL_SHADOW_BOX);
		out->color(FL_BLACK);
		out->textcolor(FL_GREEN);
		out->textsize(30);
		out->labelfont(FL_COURIER);
	}
	void setEmp(const Employee& em)
	{
		emp = em;
		what = "      Hello, \n    " + emp.getDispName();
		out->value(what.c_str());
	}
};

class AuthScreen : public Fl_Group
{
	private:
		friend class NumPad;
		Fl_Box *logo_box;
		Fl_PNG_Image *logo;
		NumPad *num_pad;
		bool manager;
		Employee current;
		bool logged;

	public:
		AuthScreen(int x, int y, int w, int h, const EmployeeBase& empbase, bool m = false, const char* l = "") :
			Fl_Group(0, 0, w, h, l), manager(m), logged(false)
		{
			logo_box = new Fl_Box(x - 75, 20, 150, 103);
			logo = new Fl_PNG_Image("postitle.png");
			num_pad = new NumPad(x - 220, 183, 440, 540, empbase, manager);
			logo_box->box(FL_OSHADOW_BOX);
			logo_box->color(fl_rgb_color(97, 32, 97));
			logo_box->image(logo);
			end();
		}
		void setCurrentUser(const Employee& em);	
		bool isLogged(){ return logged; }
		void logOff()
		{
			current = Employee();
			logged = false;
		}
		Employee currentEmp()	const { return current; }
};



// Fl_Double_Window with timer
class POSWindow : public Fl_Double_Window
{
private:
	friend class AuthScreen;
	friend class ActionPanel;
	TimeBox* the_time;
	GreetBox* greet;
	AuthScreen* auth;
	ActionPanel* actp;
	Employee currentUser;
	EmployeeBase empbase;
	Categories cats;
	int w, h;
	static void Timer_CB(void *data)
	{
		POSWindow *pw = (POSWindow*)data;
		pw->redraw();
		Fl::repeat_timeout(1.0 / 30.0, Timer_CB, data);
	}
	static void Timer_ClockB(void *data)
	{
		POSWindow *pw = (POSWindow*)data;
		pw->the_time->refresh();
		Fl::repeat_timeout(1.0, Timer_ClockB, data);
	}
	void removeWidgets()
	{
		remove(the_time);
		remove(greet);
		remove(auth);
		remove(actp);
	}
public:
	POSWindow(int ww, int hh, const char* l = 0) :
		Fl_Double_Window(ww, hh, l), w(ww), h(hh)
	{
		color(fl_rgb_color(195, 237, 187));
		Fl::add_timeout(1.0 / 30.0, Timer_CB, (void*)this);
		Fl::add_timeout(1.0, Timer_ClockB, (void*)this);
		cats.initialize("items.txt");
		empbase.initialize("emps.txt");
		actp = new ActionPanel(w - 400,h - 170,215,170);
	    greet = new GreetBox(w - 185, h - 170, 185, 95);
		the_time = new TimeBox(w - 185, h - 75, 185, 75);
		auth = new AuthScreen((w / 2), (h/2), w, h, empbase);
		remove(greet);
		remove(actp);
		remove(the_time);
		remove(auth);
		intro_Screen();
		end();
	}
	// Takes reference to window, employee database and if the login is an authentication 
	// for managerial staff
	void intro_Screen()
	{
		removeWidgets();
		add(auth);
		add(the_time);
	}
	void main_Screen()
	{
		removeWidgets();
		add(the_time);
		add(greet);
		add(actp);
	}
};


// Defined outside for forward declaration purposes
void AuthScreen::setCurrentUser(const Employee& em)
{
	current = em;
	logged = true;
	if (!manager)
	{
		POSWindow* temp = (POSWindow*)parent();
		temp->currentUser = em;
		temp->greet->setEmp(em);
		temp->main_Screen();
	}
}
void NumPad::Button_CB2(Fl_Widget *w)
{
	Fl_Button *b = (Fl_Button*)w;
	// Clear Button
	if (strcmp(b->label(), "Clear") == 0)
	{
		daty = "";
		inp->value(daty.c_str(), daty.size());
		//if (inp->inp->mark() != inp->inp->position()) inp->inp->cut();
		//else inp->inp->cut(-4);
	}
	// Enter Button
	else if (strcmp(b->label(), "Enter") == 0)
	{
		if (empdata.isEmp(inp->value()))
		{
			if (manag && empdata.getEmpByID(inp->value()).isManagement())
			{
				currentEmp = empdata.getEmpByID(inp->value());
				done = true;
				AuthScreen* temp = (AuthScreen*)parent();
				temp->setCurrentUser(currentEmp);
				daty = "";
				inp->value(daty.c_str(), daty.size());
				return;
			}
			else if (!manag)
			{
				currentEmp = empdata.getEmpByID(inp->value());
				done = true;
				AuthScreen* temp = (AuthScreen*)parent();
				temp->setCurrentUser(currentEmp);
				daty = "";
				inp->value(daty.c_str(), daty.size());
				return;
			}
		}
		daty = "";
		inp->value(daty.c_str(), daty.size());
		inp->displayWarning();
	}
	// Appends label (num) of button to input
	else
	{
		if (daty.size() != 4) daty += b->label();
		inp->value(daty.c_str(), daty.size());
	}
}
void ActionPanel::Button_RCB(Fl_Widget *w)
{
	Fl_Button *b = (Fl_Button*)w;
	POSWindow *win = (POSWindow*)b->parent()->parent();
	// New Order Button
	if (strcmp(b->label(), "New\nOrder") == 0)
	{
		//win->new_order();
	}
	// Manager Functions Button
	else if (strcmp(b->label(), "Manager\nFunctions") == 0)
	{
		//win->manag_fun();
	}
	// Appends label (num) of button to input
	else if (strcmp(b->label(), "Logout") == 0)
	{
		win->currentUser = Employee();
		win->auth->logOff();
		win->intro_Screen();
	}
	else if (strcmp(b->label(), "Utilities") == 0)
	{
		//win->utils();
	}
}


int main()
{
	// Hides Console Window (ONLY GUI SHOWN)
	// HWND hWnd = GetConsoleWindow();
	// ShowWindow(hWnd, SW_HIDE);
	POSWindow win(1200,800,"S&EPOS");
		
	win.end();
	win.show();

	return Fl::run();
}