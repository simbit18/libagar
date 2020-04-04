/* File generated by agar bundlecss */
const char *agStyleDefault_Data = 
	"/*\n"
	" * Default Agar Stylesheet.\n"
	" *\n"
	" * Colors and font attributes of parents are inherited by default (unless\n"
	" * overridden by an instance Variable or matching stylesheet definition).\n"
	" * Padding and spacing attributes are not inherited by default.\n"
	" *\n"
	" * See: AG_StyleSheet(3) and \"STYLE ATTRIBUTES\" section of AG_Widget(3).\n"
	" */\n"
	"AG_Widget {\n"
	"background-color: #0000;                   /* #RGBA */\n"
	"background-color#focused: #0000;\n"
	"background-color#disabled: #0000;\n"
	"background-color#hover: #0000;\n"
	"}\n"
	"AG_Window {\n"
	"background-color: #656565;                 /* #RRGGBB */\n"
	"background-color#focused: #666666;\n"
	"}\n"
	"AG_Box {\n"
	"padding: 2 3 2 3;                          /* top right bottom left */\n"
	"spacing: 3 2;                              /* horiz vert */\n"
	"}\n"
	"AG_Titlebar {\n"
	"font-size: 90%;\n"
	"color: rgb(40,50,60);\n"
	"color#disabled: rgb(35,35,35);\n"
	"}\n"
	"AG_Textbox {\n"
	"spacing: 5;\n"
	"}\n"
	"AG_Checkbox {\n"
	"spacing: 5;\n"
	"background-color#hover: rgb(80,80,120);\n"
	"}\n"
	"AG_Console {\n"
	"padding: 4;\n"
	"background-color: rgb(0,0,0);\n"
	"background-color#hover: rgb(0,0,30);\n"
	"background-color#focused: rgb(5,5,5);\n"
	"background-color#disabled: rgb(10,10,10);\n"
	"text-color: rgb(240,240,240);\n"
	"}\n"
	"AG_Button {\n"
	"padding: 5 10 5 10;\n"
	"}\n"
	"AG_Editable {\n"
	"background-color: #0000;\n"
	"}\n"
	"AG_Icon {\n"
	"spacing: 4 0;\n"
	"}\n"
	"AG_Label {\n"
	"padding: 2 6 2 6;\n"
	"background-color: #0000;\n"
	"background-color#focused: #0000;\n"
	"}\n"
	"AG_Menu {\n"
	"padding: 2 5 2 5;\n"
	"color: rgb(70,70,70);\n"
	"color#disabled: rgb(40,40,110);\n"
	"selection-color: rgb(40,40,110);\n"
	"text-color#disabled: rgb(170,170,170);\n"
	"}\n"
	"AG_MenuView {\n"
	"padding: 4 8 4 8;\n"
	"color: rgb(70,70,70);\n"
	"selection-color: rgb(40,40,110);\n"
	"color#disabled: rgb(40,40,110);\n"
	"text-color#disabled: rgb(170,170,170);\n"
	"}\n"
	"AG_Pane {\n"
	"line-color#hover: rgb(200,200,180);\n"
	"}\n"
	"AG_Radio {\n"
	"spacing: 1;\n"
	"background-color#hover: rgb(80,80,120);\n"
	"}\n"
	"AG_ProgressBar {\n"
	"font-family: league-gothic;\n"
	"font-size: 90%;\n"
	"padding: 2;\n"
	"}\n"
	"AG_Separator {\n"
	"padding: 4;\n"
	"}\n"
	"";

AG_StaticCSS agStyleDefault = {
	"agStyleDefault",
	1850,
	&agStyleDefault_Data,
	NULL
};
