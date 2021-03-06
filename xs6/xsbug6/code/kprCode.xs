<?xml version="1.0" encoding="UTF-8"?>
<!--
|     Copyright (C) 2010-2016 Marvell International Ltd.
|     Copyright (C) 2002-2010 Kinoma, Inc.
|
|     Licensed under the Apache License, Version 2.0 (the "License");
|     you may not use this file except in compliance with the License.
|     You may obtain a copy of the License at
|
|      http://www.apache.org/licenses/LICENSE-2.0
|
|     Unless required by applicable law or agreed to in writing, software
|     distributed under the License is distributed on an "AS IS" BASIS,
|     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
|     See the License for the specific language governing permissions and
|     limitations under the License.
-->
<package script="true">
	<import href="kpr.xs" link="dynamic"/>
	<patch prototype="KPR">
		<object name="code" prototype="KPR.content">
			<function name="get columnCount" c="KPR_code_get_columnCount"/>
			<function name="get columnWidth" c="KPR_code_get_columnWidth"/>
			<function name="get editable" c="KPR_code_get_editable"/>
			<function name="get length" c="KPR_code_get_length"/>
			<function name="get lineCount" c="KPR_code_get_lineCount"/>
			<function name="get lineHeight" c="KPR_code_get_lineHeight"/>
			<function name="get resultCount" c="KPR_code_get_resultCount"/>
			<function name="get selectable" c="KPR_code_get_selectable"/>
			<function name="get selectionBounds" c="KPR_code_get_selectionBounds"/>
			<function name="get selectionOffset" c="KPR_code_get_selectionOffset"/>
			<function name="get selectionLength" c="KPR_code_get_selectionLength"/>
			<function name="get selectionString" c="KPR_code_get_selectionString"/>
			<function name="get string" c="KPR_code_get_string"/>
			<function name="get type" c="KPR_code_get_type"/>

			<function name="set active" c="KPR_code_set_active"/>
			<function name="set editable" c="KPR_code_set_editable"/>
			<function name="set selectable" c="KPR_code_set_selectable"/>
			<function name="set string" params="it" c="KPR_code_set_string"/>
			<function name="set type" params="it" c="KPR_code_set_type"/>
			
			<function name="extract" params="offset, length" c="KPR_code_extract"/>
			<function name="find" params="pattern, mode" c="KPR_code_find"/>
			<function name="findAgain" params="direction" c="KPR_code_findAgain"/>
			<function name="findBlock" params="offset" c="KPR_code_findBlock"/>
			<function name="findLineBreak" params="offset, forward" c="KPR_code_findLineBreak"/>
			<function name="findWordBreak" params="offset, forward" c="KPR_code_findWordBreak"/>
			<function name="hitOffset" params="x, y" c="KPR_code_hitOffset"/>
			<function name="insert" params="text" c="KPR_code_insert"/>
			<function name="isSpace" params="offset" c="KPR_code_isSpace"/>
			<function name="locate" params="offset" c="KPR_code_locate"/>
			<function name="replace" params="replacement" c="KPR_code_replace"/>
			<function name="replaceAll" params="replacement" c="KPR_code_replaceAll"/>
			<function name="select" params="offset, length" c="KPR_code_select"/>
			<function name="tab" params="direction" c="KPR_code_tab"/>
			<function name="writeMessage" params="message" c="KPR_code_writeMessage"/>
		</object>
	</patch>
	<function name="Code" params="data, dictionary" prototype="KPR.code" c="KPR_Code"/>
	<program>
		Code.template = template;
	</program>

]</package>

