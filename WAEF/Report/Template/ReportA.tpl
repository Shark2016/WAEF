<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gbk" />
<link href="SIStyle.css" rel="stylesheet" type="text/css" />
<link href="SITextStyle.css" rel="stylesheet" type="text/css" />
</head>
<body>
<div class="bodydiv">

  <div id="divvulnerabilitysection">
		<div style="height:36px; overflow: hidden">
			<div class="sectionheadgrey" style="height:36px;">
				<img src="./Graphics/ScanInfo/grey-u-l.gif" width="12" height="36" style="float:left"/>		
			</div>
			<div class="sectionheadtext" style="height:36px; width:100%; position:relative; top:-36px; white-space:nowrap; overflow:hidden">
				<img src="./Graphics/transparent-dot.gif" width="16" height="1"/><img src="./Graphics/ScanInfo/vuln-chart.gif" width="16" height="16" vspace="4" align="absmiddle"/> 漏洞信息
			</div>
			<div style="height:36px; position:relative; top:-72px;">
				<img src="./Graphics/ScanInfo/grey-u-r.gif" width="12" height="36" style="float:right"/>
			</div>
		</div>
    <div class="infotext bgrey">
      <div class="infotexttitle bgrey"> 漏洞报告</div>
      <table border="0" cellpadding="0" cellspacing="0">
        <tr>
          <td width="150" class="boldnavy">总计</td>
          <td width="50" align="right" class="boldnavy monospacefont" id="tdtotalalerts">{%VUL_COUNT%}</td>
          <td width="212">&nbsp;</td>
        </tr>
        <tr>
          <td width="150" class="boldred"><img src="./Graphics/vuln-red.png" width="16" height="16" hspace="6" vspace="2" align="absmiddle" />高危</td>
          <td width="50" align="right" class="monospacefont" id="tdhighalerts">{%VUL_HIGH%}</td>
          <td width="212"><img src="./Graphics/ScanInfo/red-dot.gif" name="imghighmeter" width="{%VUL_HIGH%}0" height="15" hspace="6" id="imghighmeter" /></td>
        </tr>
        <tr>
          <td width="150" class="boldorange"><img src="./Graphics/vuln-orange.png" width="16" height="16" hspace="6" vspace="2" align="absmiddle" />中危</td>
          <td width="50" align="right" class="monospacefont" id="tdmediumalerts">{%VUL_MEDIUM%}</td>
          <td width="212"><img src="./Graphics/ScanInfo/orange-dot.gif" name="imgmediummeter" width="{%VUL_MEDIUM%}0" height="15" hspace="6" id="imgmediummeter" /></td>
        </tr>
        <tr>
          <td width="150" class="boldgrey"><img src="./Graphics/vuln-blue.png" width="16" height="16" hspace="6" vspace="2" align="absmiddle" />低危</td>
          <td width="50" align="right" class="monospacefont" id="tdlowlerts">{%VUL_LOW%}</td>
          <td width="212"><img src="./Graphics/ScanInfo/blue-dot.gif" name="imglowmeter" width="{%VUL_LOW%}0" height="15" hspace="6" id="imglowmeter" /></td>
        </tr>
        <tr>
          <td width="150" class="boldgreen"><img src="./Graphics/vuln-green.png" width="16" height="16" hspace="6" vspace="2" align="absmiddle" />提醒</td>
          <td width="50" align="right" class="monospacefont" id="tdinfoalerts">{%VUL_INFO%}</td>
          <td width="212"><img src="./Graphics/ScanInfo/green-dot.gif" name="imginfometer" width="{%VUL_INFO%}0" height="15" hspace="6" id="imginfometer" /></td>
        </tr>
      </table>
    </div>
    <div class="sectionfootgrey"> <img src="./Graphics/ScanInfo/grey-l-l.gif" width="12" height="18" style="float:left"/> <img src="./Graphics/ScanInfo/grey-l-r.gif" width="12" height="18" style="float:right"/> </div>
  </div>


</div>
</body>
</html>
