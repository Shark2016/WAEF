<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gbk" />
<link href="SIStyle.css" rel="stylesheet" type="text/css" />
<link href="SITextStyle.css" rel="stylesheet" type="text/css" />
</head>
<body>
<div class="bodydiv">

  <div id="divscansection">
		<div style="height:36px; overflow: hidden">
			<div class="sectionhead{%VUL_COLOR%}" style="height:36px;">
				<img src="./Graphics/ScanInfo/{%VUL_COLOR%}-u-l.gif" width="12" height="36" style="float:left"/>		
			</div>
			<div class="sectionheadtext" style="height:36px; width:100%; position:relative; top:-36px; white-space:nowrap; overflow:hidden">
				<img src="./Graphics/transparent-dot.gif" width="16" height="1"/><img src="./Graphics/ScanInfo/server-info.gif" width="16" height="16" vspace="4" align="absmiddle"/> {%VUL_NAME%}
			</div>
			<div style="height:36px; position:relative; top:-72px;">
				<img src="./Graphics/ScanInfo/{%VUL_COLOR%}-u-r.gif" width="12" height="36" style="float:right"/>
			</div>
		</div>
    <div class="infotext b{%VUL_COLOR%}">
      <div id="divscansectioninfo">
        <div class="infotexttitle b{%VUL_COLOR%}">漏洞描述</div>
        <div>
          <table border="0" cellspacing="0" cellpadding="0">
            <tr>
              <td width="175" valign="top" class="lefttexts">路径</td>
              <td valign="top" class="monospacefont" id="tdtargetname">{%VUL_PATH%}</td>
            </tr>
            <tr>
              <td width="175" valign="top" class="lefttexts">等级</td>
              <td valign="top" class="monospacefont" id="tdserverbanner">{%VUL_LEVEL%}</td>
            </tr>
          </table>
        </div>
        <div id="divPortScanProgress">
          <div class="infotexttitle b{%VUL_COLOR%}">漏洞细节</div>
          <table border="0" cellspacing="0" cellpadding="0">
            <tr id="trPortScanProgress">
              <td valign="top" class="lefttexts"> {%VUL_DESC%}</td>
            </tr>
          </table>
        </div>
        <div id="divPortScanProgress">
          <div class="infotexttitle b{%VUL_COLOR%}">修复方案</div>
          <table border="0" cellspacing="0" cellpadding="0">
            <tr id="trPortScanProgress">
              <td valign="top" class="lefttexts"> {%VUL_FIX%}</td>
            </tr>
          </table>
        </div>
      </div>
    </div>
    <div class="sectionfoot{%VUL_COLOR%}"> <img src="./Graphics/ScanInfo/{%VUL_COLOR%}-l-l.gif" width="12" height="18" style="float:left"/> <img src="./Graphics/ScanInfo/{%VUL_COLOR%}-l-r.gif" width="12" height="18" style="float:right"/> </div>
  </div>


</div>
</body>
</html>
