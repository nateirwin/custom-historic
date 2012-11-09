function newWindow(page,name,scroll) {
	var winl = 5;
	var wint = 5;
	//winprops = 'height='+hght+',width='+wth+',top='+wint+',left='+winl+',scrollbars='+scroll+',resizable';
	winprops = ',top='+wint+',left='+winl+',scrollbars='+scroll+',resizable';
	win = window.open(page, name, winprops);

	if (win.location && !win.closed) {
        win.focus();
    }
}
