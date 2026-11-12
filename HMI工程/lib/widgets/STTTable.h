/*
 * STTTable.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"
#include "widgets/STTPage.h"

namespace stt{

/**
 * @ingroup widget
 * @brief 表格
 */
class STTTable: public STTWidget {
public:
	/**
	 * @brief 列结构
	 */
	struct Column{
		string key;		//列字段名，与数据源的列名一致
		string label;	//列显示的标题
		uint16_t width;	//列宽度
		uint8_t fac;	//小数位
	};

public:
	/**
	 * @HideCA
     * @brief 表格控件构造函数
     * @param rt		运行时环境
     * @param par		父控件
     * @param id		控件说明
	 */
	STTTable(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 */
	virtual ~STTTable();

    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x			相对坐标x
     * @param y			相对坐标y
     * @param w			宽度 小于0时，跳过xywh设置(保持原值)
     * @param h			高度 小于0时，跳过xywh设置(保持原值)
     * @param cols		列定义
     * @param rowcnt	总行数(=表头1行+数据n行 , 小于0时，跳过行数设置(保持原值))
     * @param wrap		单元格内容是否自动换行
     * @param tabletype	表格类型：默认为15~未指定 0~实时采样表 1~历史采样表 2~事件表 3~报警表 4~配方表 5~操作日志 6~数据查询
     */
	void setMainArg(int x, int y, int w, int h, vector<Column>& cols, int rowcnt, bool wrap=true, int tabletype = 15);
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x			相对坐标x
     * @param y			相对坐标y
     * @param w			宽度
     * @param h			高度
     * @param cols		列定义
     * @param rowcnt	数据行数（ = 总行数-1 (表头)）
     * @param wrap		单元格内容是否自动换行
     */
	void setMainArg(int x, int y, int w, int h, const string& cols="", int rowcnt=10, bool wrap=true, int tabletype = -1);
    /**
     * @LEVEL2
     * @brief 设置标题行样式。
     * @param c1		表头单元格的主色
     * @param c2		表头单元格的渐变终止色
     * @param dir		表头单元格的渐变方向
     * @param w_border	表头单元格的边框线宽
     * @param c_border	表头单元格的边框线色
     * @param padt		表头单元格的上边距
     * @param padb		表头单元格的下边距
     * @param padl		表头单元格的左边距
     * @param padr		表头单元格的右边距
     * @param c_font	表头单元格文字颜色
     * @param fontsize	表头单元格文字大小
     * @param fontname	表头单元格文字字体
     */
    void setHeadCellStyle(int c1, int c2, int dir, int w_border, int c_border, int padt, int padb, int padl, int padr, int c_font, int fontsize, const string& fontname, int fontstyle=0);
    /**
     * @LEVEL2
     * @brief 设置选中行cell样式。
     * @param c1		选中行单元格的主色
     * @param c2		选中行单元格的渐变终止色
     * @param dir		选中行单元格的渐变方向
     * @param w_border	选中行单元格的边框线宽
     * @param c_border	选中行单元格的边框线色
     * @param padt		选中行单元格的上边距
     * @param padb		选中行单元格的下边距
     * @param padl		选中行单元格的左边距
     * @param padr		选中行单元格的右边距
     * @param c_font	选中行单元格文字颜色
     * @param fontsize	选中行单元格文字大小
     * @param fontname	选中行单元格文字字体
     */
    void setSelCellStyle(int c1, int c2, int dir, int w_border, int c_border, int padt, int padb, int padl, int padr, int c_font, int fontsize, const string& fontname, int fontstyle=0);
    /**
     * @LEVEL2
     * @brief 设置数据行cell样式。
     * @param c1		数据单元格的主色
     * @param c2		数据单元格的渐变终止色
     * @param dir		数据单元格的渐变方向
     * @param w_border	数据单元格的边框线宽
     * @param c_border	数据单元格的边框线色
     * @param padt		数据单元格的上边距
     * @param padb		数据单元格的下边距
     * @param padl		数据单元格的左边距
     * @param padr		数据单元格的右边距
     * @param c_font	数据单元格文字颜色
     * @param fontsize	数据单元格文字大小
     * @param fontname	数据单元格文字字体
     */
    void setDataCellStyle(int c1, int c2, int dir, int w_border, int c_border, int padt, int padb, int padl, int padr, int c_font, int fontsize, const string& fontname, int fontstyle=0);
    /**
     * @LEVEL0
     * @brief 设置数据行数
     * @param cnt 数据行数，即表格总行数-1(表头1行)
     * @note setRowCnt后调用rcpBind，则行数为配方数据的行数
     */
    STTTable*  setRowCnt(uint16_t cnt);
    /**
     * @LEVEL0
     * @brief 设置指定列宽度
     * @param col_id 	列索引
     * @param w			列宽度
     */
    STTTable* setColWidth(uint16_t col_id, int16_t w);
    /**
     * @LEVEL1
     * @brief 设置单元格控制属性
     * @param row	整个表的行索引。即第1行(表头)的索引为0，第2行(或称表体的第1行)的索引为1
     * @param col	列索引
     * @param ctrl 	属性：1~向右合并相同内容的单元格
     */
    STTTable* addCellCtrl(uint16_t row, uint16_t col, uint8_t ctrl=1);
    /**
     * @LEVEL0
     * @brief 设置单元格内容
     * @param row	表体(即不计第1行,因为第1行是表头)的行索引
     * @param col	列索引
     * @param txt	内容
     */
    STTTable* setCellValue(uint16_t row, uint16_t col, const string& txt);
    /**
     * @LEVEL0
     * @brief 获取绑定的数据对象
     * @return
     */
    TTable* getData();
    /**
     * @LEVEL1
     * @brief 变量绑定
     * @param data
     */
    void bindData(IVariable * data);
    /**
     * @LEVEL1
     * @brief 解绑变量，并使用自带TTable数据
     */
    void disBindData();
    /**
     * @LEVEL1
     * @brief 变量绑定
     */
    void bindData(shared_ptr<IVariable> data);
    /**
     * @HideCA
     * @brief 数据处理
     * @param org
     * @param data
     */
    void onDataChanged(SttObject* org, IVariable* data);
    /**
     * @HideCA
     * @brief 内部函数
     **/
    void onHistoryDateChanged(SttObject * org, IVariable* data);
    void onHistoryDate2Changed(SttObject * org, IVariable* data);
    /**
     * @LEVEL0
	 * @brief 选中项，不触发点击事件
	 * @param i 表体的行索引(如为0，表示整个表的第2行(因为第1行是表头))
	 */
	void setSelected(int i);
	/**
	 * @brief 获取当前分页序号
	 * @return
	 */
	int getPageIndex();
	/**
	 * @brief 获取总分页数
	 * @return
	 */
	int getPageCnt();
	/**
	 * @brief 设置分页大小
	 * @param size
	 */
	void setPageSize(int size);
	/**
	 * @brief 获取每页记录数
	 * @return
	 */
	int getPageSize();
	/**
	 * @brief 获取总记录数
	 * @return
	 */
	int getRecordCnt();
	/**
	 * @brief 跳转第一页
	 * @note 资料取样实时模式的数据源是TTable类型，不能跳转；配方表也不能使用
	 * @return
	 */
	void navFirst();
	/**
	 * @brief 跳转下一页
	 * @note 资料取样实时模式的数据源是TTable类型，不能跳转；配方表也不能使用
	 */
	void navNext();
	/**
	 * @brief 跳转上一页
	 * @note 资料取样实时模式的数据源是TTable类型，不能跳转；配方表也不能使用
	 */
	void navBack();
	/**
	 * @brief 跳转最后一页
	 * @note 资料取样实时模式的数据源是TTable类型，不能跳转；配方表也不能使用
	 */
	void navLast();
	/**
	 * @brief (关联变量)分页记录数
	 * @param v
	 */
	void bindPageSize(Ivariable v);
	/**
	 * @brief (关联变量)总记录数
	 * @param v
	 */
	void bindRecordCnt(Ivariable v);
	/**
	 * @brief (关联变量)当前页序号
	 * @param v
	 */
	void bindPageIndex(Ivariable v);
	/**
	 * @brief (关联变量)总页数
	 * @param v
	 */
	void bindPageCnt(Ivariable v);
	/**
	 * @LEVEL1
	 * @brief 绑定起始日期，对采样或事件有效(一个历史数据的日期变量yyyymmdd)
	 * @param v
	 */
	void bindHistoryDate(Ivariable v);
	/**
	 * @LEVEL1
	 * @brief 绑定截止日期，仅对事件有效(一个历史数据的日期变量yyyymmdd)
	 * @param v
	 */
	void bindHistoryDate2(Ivariable v);
	/**
	 * @LEVEL1
	 * @brief 设置历史曲线日期，小于20240101表示当天
	 * @param ymdstart 起始日期 对采样和事件有效
	 * @param ymdend	截止日期 仅对事件有效
	 * @note 对采样表和事件表有效
	 */
	void setHistoryDate(int ymdstart = 0, int ymdend = 0);
	/**
	 * @brief 获取选中行
	 * @return 选中行序号
	 */
	int getSelectedRow();
	/**
	 * @brief 获取选中列
	 * @return 选中列序号
	 */
	int getSelectedCol();
	/**
	 * @LEVEL1
	 * @brief 设置时间列的显示模式 0~时间(默认) 1~日期 2~日期+时间
	 * @param datemode 时间列的显示模式 0~时间(默认) 1~日期 2~日期+时间
	 */
	void setDateMode(int datemode);

public:
    /**@HideCA*/
    void* mMux;
    /**@HideCA 是否报警表*/
	bool isWarningTable();
private:
    /**@HideCA 刷新整个容器*/
	void invalidateThis();
	/**@HideCA 刷新表头*/
	void invalidateTableHead();
	/**@HideCA 刷新表体容器*/
	void invalidatePanelBody();
	/**@HideCA 刷新表体*/
	void invalidateTableBody();
    /**@HideCA 语言切换*/
    void onLanguageChanged(SttObject * org, IVariable* data);
public:
    /**
     * @LEVEL1
     * @brief 设置采样数据源，等同于SampleMgr::getInstance(p_rt)->bind
     * @param sampleid 采样id, [1..n]。当为空时，解绑
     * @param yyyymmdd 采样日期，为空时表示今天(实时数据)
     */
    void setDataSource(const string& sampleid, const string& yyyymmdd="", bool isDesc=true);
    /**
     * @LEVEL1
     * @brief 设置事件或报警数据源，等同于EventMgr::getInstance(p_rt)->bind
     * @param yyyymmdd 事件日期，为空时表示今天
     * @param warning 是则报警表，否则事件表
     * @param catlog	事件或报警的类别0~99
     * @param level		事件事报警的级别：(1低级|2一般|4高级|8紧急)
     * @param isDesc	是否倒序
     */
    void setDataSourceOfEvent(const string& ymdstart,const string& ymdend
    		,bool warn, int cata1, int cata2, int level = 0, bool isDesc=true);
    /**
     * @LEVEL2
     * @brief 增加自定义颜色。需要调用STTWidget::invalidate()刷新显示。
     * @param isbody 整个表由表头(第1行)和表体两个单表组成,取值false~表头 true~表体
     * @param isfg false~背景色，true~前景色(字体颜色)
     * @param irow 行索引(表头和表体分别)
     * @param icol 列索引
     * @param icolor 指定的颜色
     */
    void addCustomColor(bool isbody,bool isfg,uint32_t irow,uint32_t icol, uint32_t icolor);
    /**
     * @LEVEL2
     * @brief 删除指定颜色。需要调用STTWidget::invalidate()刷新显示。
     * @param isbody 整个表由表头和表体两个单表组成,取值false~表头 true~表体
     * @param isfg false~背景色，true~前景色(字体颜色)
     * @param irow 行索引
     * @param icol 列索引
     */
    void removeCustomColor(bool isbody,bool isfg,uint32_t irow,uint32_t icol);
    /**
     * @LEVEL2
     * @brief 删除指定颜色。需要调用STTWidget::invalidate()刷新显示。
     * @param isbody 整个表由表头和表体两个单表组成,取值false~表头 true~表体
     * @param isfg false~背景色，true~前景色(字体颜色)
     */
    void clearCustomColor(bool isbody,bool isfg);
    /**
     * @LEVEL2
     * @brief 取指定颜色，颜色值在iret参数中
     * @param isbody 整个表由表头和表体两个单表组成,取值false~表头 true~表体
     * @param isfg false~背景色，true~前景色(字体颜色)
     * @param irow 行索引
     * @param icol 列索引
     * @param iret [输出]取得颜色值
     * @return 为true时,iret返回有效
     */
    bool getCustomColor(bool isbody,bool isfg,uint32_t irow,uint32_t icol, uint32_t& iret);
public:
    /**
     * @LEVEL2
     * @brief 编辑配方表格数据
     * @param r 表体的行索引
     * @param c 列索引(当c=0时，调用无效；当列未绑定变量时，调用无效)
     */
    void editRCPCell(int r, int c);
    /**
     * @LEVEL2
     * @brief 新增
     * @return
     */
    bool rcpAdd();
    /**
     * @LEVEL2
     * @brief 插入行
     * @return
     */
    bool rcpInsert();
    /**
     * @LEVEL2
     * @brief 拷贝选中行
     * @return
     */
    bool rcpCopy();
    /**
     * @LEVEL2
     * @brief 删除选中行
     * @return
     */
    bool rcpDel();
    /**
     * @LEVEL2
     * @brief 保存配方数据
     * @return
     */
    bool rcpSave();
    /**
     * @LEVEL2
     * @brief 导出配方csv格式
     * @param path 模拟器中相对于./data/的文件路径；屏中相对于U盘根目录的文件路径。
     * 			   如"myrcp.csv"；若带目录如"dir/myrcp.csv"（应确保目录dir已存在）
     * @return
     */
    bool rcpExportAs(const string& path);
    /**
     * @LEVEL2
     * @brief 导入配方
     * @param path	模拟器中相对于./data/的文件路径；屏中相对于U盘根目录的文件路径。
     * 				如"myrcp.csv"；若带目录如"dir/myrcp.csv"（应确保目录dir已存在）
     * @return
     */
    bool rcpImport(const string& path);
    /**
     * @brief 同RcpMgr::bind(this, rcpname);
     */
    bool rcpBind(const string& rcpname);
    /**
     * @LEVEL2
     * @brief 将当前选中的行与上一行交换位置
     */
    void moveUp();
    /**
     * @LEVEL2
     * @brief 将当前选中的行与下一行交换位置
     */
    void moveDown();
    /**
     * @LEVEL2
     * @brief	设置表格配色模式
     * @param mode	0~渐变 1~隔行
     */
    void setColorMode(int mode);
    /**
     * @LEVEL0
     * @brief	设置表头文本
     * @param	colindex	列索引，从0开始
     * @param	text		文本
     * @param	frac		该列数据的小数位。小于0时，跳过设置
     */
    void setHeaderText(int colindex, string text, int frac=0);
    /**
     * @LEVEL0
     * @brief	获取表头文本
     * @param	colindex	列索引
     */
    string getHeaderText(int colindex);
    /**
     * @LEVEL0
     * @brief	导出为csv到u盘(模拟器到data下)
     * @note	1.采样表(实时)：导出的是内存数据
     * 			2.采样表(历史)：绑定日期的采样数据
     * 			3.事件表：全部事件数据记录
     * 			4.报警表：报警数据
     * 			5.配方表：当前绑定的配方数据, rcp_xx.csv
     * 			6.操作日志表：全部操作日志
     * 			7.数据查询表：查询语句得到的数据
     */
    bool exportCSV2UDisk();
    /**
     * @LEVEL0
     * @brief 开启拖拽模式
     */
    virtual void enableDrag();
    /**
     * @LEVEL0
     * @brief 设置单元格对齐方式
     * @param row 表体(即不计第1行的表头)的行索引值(小于0时，所有行)
     * @param col 列索引值(小于0时，所有列)
     * @param align 对齐 1左 2中 3右
     */
    void setCellAlign(int row,int col,int align);
    /**
     * @LEVEL0
     * @brief 取某单元格的对齐方式
     * @return 对齐 1左 2中 3右
     */
    int getCellAlign(int row,int col);
    /**
     * @LEVEL0
     * @brief 设置单元格双击事件是否禁用
     * @param row 表体(即不计第1行的表头)行索引值(小于0时，对所有行禁用双击)
     * @param col 列索引值(小于0时，对所有列禁用双击)
     * @param disable 是否禁用双击事件，默认true
     */
    void setCellDoubleClickDisable(int row,int col,bool disable=true);
    /**
	 * @LEVEL0
	 * @brief 取某单元格的双击事件是否禁用
	 * @return 是否禁用双击事件，默认true
	 */
    bool getCellDoubleClickDisable(int row,int col);
};

typedef STTTable* SttTable;

}
