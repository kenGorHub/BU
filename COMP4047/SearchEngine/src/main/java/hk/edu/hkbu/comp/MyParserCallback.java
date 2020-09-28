package hk.edu.hkbu.comp;

import javax.swing.text.html.*;
import javax.swing.text.html.HTML.*;
import javax.swing.text.html.HTMLEditorKit.*;
import javax.swing.text.html.parser.*;
import javax.swing.text.*;

class MyParserCallback extends HTMLEditorKit.ParserCallback {
	public String content = new String();

	@Override
	public void handleText(char[] data, int pos) {
		content += " " + new String(data);
	}
}
