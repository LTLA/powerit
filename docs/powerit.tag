<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>core.hpp</name>
    <path>powerit/</path>
    <filename>core_8hpp.html</filename>
    <class kind="struct">powerit::Options</class>
    <class kind="struct">powerit::Result</class>
    <namespace>powerit</namespace>
  </compound>
  <compound kind="file">
    <name>powerit.hpp</name>
    <path>powerit/</path>
    <filename>powerit_8hpp.html</filename>
    <includes id="simple_8hpp" name="simple.hpp" local="yes" import="no" module="no" objc="no">simple.hpp</includes>
    <includes id="core_8hpp" name="core.hpp" local="yes" import="no" module="no" objc="no">core.hpp</includes>
    <namespace>powerit</namespace>
  </compound>
  <compound kind="file">
    <name>simple.hpp</name>
    <path>powerit/</path>
    <filename>simple_8hpp.html</filename>
    <includes id="core_8hpp" name="core.hpp" local="yes" import="no" module="no" objc="no">core.hpp</includes>
    <namespace>powerit</namespace>
  </compound>
  <compound kind="struct">
    <name>powerit::Options</name>
    <filename>structpowerit_1_1Options.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>iterations</name>
      <anchorfile>structpowerit_1_1Options.html</anchorfile>
      <anchor>a840c57cd50dc00391798227b1e1b76d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>tolerance</name>
      <anchorfile>structpowerit_1_1Options.html</anchorfile>
      <anchor>a38186e948820f0613efd56c0aaea9682</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structpowerit_1_1Options.html</anchorfile>
      <anchor>a4a1f98bf74c51140d86d7845bec4701a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>powerit::Result</name>
    <filename>structpowerit_1_1Result.html</filename>
    <templarg>typename Data_</templarg>
    <member kind="variable">
      <type>Data_</type>
      <name>value</name>
      <anchorfile>structpowerit_1_1Result.html</anchorfile>
      <anchor>a58502abff7d9e05383d015a6c7eec8ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>iterations</name>
      <anchorfile>structpowerit_1_1Result.html</anchorfile>
      <anchor>af2bd95e01d14a0ecfe856f7edd92193f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>powerit</name>
    <filename>namespacepowerit.html</filename>
    <class kind="struct">powerit::Options</class>
    <class kind="struct">powerit::Result</class>
    <member kind="function">
      <type>void</type>
      <name>fill_starting_vector</name>
      <anchorfile>namespacepowerit.html</anchorfile>
      <anchor>ae2dca12dec07f58cd18e35be2364ed3d</anchor>
      <arglist>(size_t order, Data_ *vector, Engine_ &amp;engine)</arglist>
    </member>
    <member kind="function">
      <type>Result&lt; Data_ &gt;</type>
      <name>compute_core</name>
      <anchorfile>namespacepowerit.html</anchorfile>
      <anchor>ae142930faf055737e8a45cba10f7efef</anchor>
      <arglist>(size_t order, Multiply_ multiply, Data_ *vector, const Options &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>Result&lt; Data_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacepowerit.html</anchorfile>
      <anchor>a807c3e8dc3a452ccfddfb19f481b9389</anchor>
      <arglist>(size_t order, const Data_ *matrix, bool row_major, Data_ *vector, Engine_ &amp;engine, const Options &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>Result&lt; Data_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacepowerit.html</anchorfile>
      <anchor>a74f84d010735c5e3acbdcab0a0915ad6</anchor>
      <arglist>(size_t order, const Data_ *matrix, bool row_major, Data_ *vector, const Options &amp;opt)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Power iterations in C++</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Power iterations in C++">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
