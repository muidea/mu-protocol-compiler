mu-protocol-compiler
====================

a protocol script compiler for c++

1.	功能说明
MUPCompiler是自动生成通讯协议代码的代码生成器软件。用户定义协议描述文件，MUPCompiler自动将其编译成通讯协议代码，当前版本只支持C++语言。

MUPCompiler的协议描述文件采用类C语言语法描述。

1.1.	支持的协议格式类型
MUPCompiler支持定义结构体类型（mustruct），列表类型（mulist、muvector），和类类型（muclass）。结构体类型（mustruct），用于定义包含多个内置变量，以及自定义结构体类型的变量的协议。列表类型（mulist、muvector）用于定义包含多个相同类型数据的协议。类类型（muclass）用于定义基于派生类型数据的协议
1.2.	支持的协议字段类型
MUPCompiler支持以下数据类型。
INT8、UINT8、INT16、UINT16、INT32、UINT32、INT、UINT、FLOAT32，FLOAT64、BOOL、CHAR、LONG
支持stl的string,vector和list。
1.3.	描述文件组织结构
├─package
│  ├─package
│  ├─package
│  ├─package
│  │  ├─module
package：目录或描述文件
module：描述文件中定义的协议类型

★ 描述文件扩展名为.mps
2.	语法规则
MUPCompiler的协议描述语法如下：
#descriptions or comments						①
[import  [package]*. Module;]*					②
mustruct moduleName							③
{
[varType varName]*;							④
};
mustruct moduleName[：BaseName]*				⑤
{
[varType varName]*;							
};

mulist moduleName = typeName;					⑥
muvector moduleName = typeName;				⑦
moduleName = typeName;						⑧


#descriptions or comments
①描述信息或者注释
以#开始的表示注释

[import  [package]*. Module;]*
② 引入的外部模块
Import 关键字，如果存在引入外部模块，则该字段必填
package:外部模块所属包名，允许存在多级
module:外部模块名，

mustruct moduleName
③定义的协议模块。
Mustruct 关键字
ModuleName 模块名

[varType varName]*;
④定义协议包含的字段。
VarType 字段类型，必须为之前定义的Module名或引入的Module名
VarName 字段名
协议字段可以定义多个


mustruct moduleName[：BaseName]*	
⑤基类类名
BaseName 基类名称

mulist moduleName = typeName;					
⑥定义一个list
ModuleName为定义的list名称
TypeName定义为list中的数据类型，必须为之前定义的Module名或引入的Module名

muvector moduleName = typeName;				
⑦定义一个vector
ModuleName为定义的vector名称
TypeName定义为vector中的数据类型，必须为之前定义的Module名或引入的Module名

moduleName = typeName;						
⑧重定义
ModuleName为重定义名称
TypeName源数据类型，必须为之前定义的Module名或引入的Module名

3.	使用方法
MUPCompiler的使用步骤如下：
1、编写协议描述文件
2、使用MUPCompiler编译协议描述文件
3、使用编译生成的C++文件。

命令行：
Usage: MUPCompiler [<参数>...]
<参数描述>
    /file: 协议脚本文件. 例如test.mps，该参数为必填参数
    /include: 协议脚本包含路径，该参数可选参数
    /namespace: 生成代码所在的命名空间，该参数为可选参数
    /output: 生成代码的输出目录，该参数为可选参数
4.	常见问题
1、	默认的输出目录为当前的工作目录。
2、	Import脚本默认目录为当前的工作目录。mps文件中import的module查找顺序为mps文件的所在目录，当前目录，import目录。
3、	mps文件可以指定绝对路径，也可以指定相对路径，推荐使用相对路径。
