/*
*********************************************************************************************************
*	                                  
*	模块名称 : 全拼音输入法
*	说    明 : 这部分汉字表和检索函数来自网络，使用本工程请保证此文件的属性是UCS-2 Little格式
*********************************************************************************************************
*/


#include "pysearch.h"
#include "string.h"


/*
*********************************************************************************************************
*	                         汉字列表
*********************************************************************************************************
*/
/*** A ***/
const unsigned char PY_mb_space []={""}; 
const unsigned char PY_mb_a     []={"啊阿吖呵腌嗄锕"}; 
const unsigned char PY_mb_ai    []={"爱唉哎艾挨矮癌埃哀碍愛暧嗳隘霭捱蔼皑嫒噯瑷嗌毐欸僾薆嬡壒鑀砹敳礙曖锿啀璦瞹娭藹譪呆溾昹騃靄皚鴱溰餲"};
const unsigned char PY_mb_an    []={"按俺安案暗岸鞍氨庵胺黯铵谙桉唵闇垵盦儑侒鹌犴埯厂广揞荌鮟諳銨錌豻韽萻啽峖馣腤雸鵪"};
const unsigned char PY_mb_ang   []={"昂盎肮昻骯醠"};
const unsigned char PY_mb_ao    []={"奥熬澳傲凹敖袄嗷獒拗坳鳌岙螯翱遨骜媪懊奧璈鏊鏖廒奡芺聱鰲謷墺擙襖鼇镺扷嶴岰滶隞爊蔜柪艹"};

/*** B ***/
//const unsigned char PY_mb_ba    []={"八爸拔罢跋霸疤吧坝把巴捌灞芭扒坝叭靶笆疤耙捌粑茇岜鲅钯魃"};
const unsigned char PY_mb_ba      []={"八捌拔跋霸爸吧坝巴把罢扒芭疤靶抜叭耙粑钯笆茇岜魃罷仈鲅犮菝灞壩覇矲豝胈朳弝蚆釟軷釛鼥"};
//bai
//const unsigned char PY_mb_bai   []={"百白摆败柏拜佰伯稗捭呗掰"};
const unsigned char PY_mb_bai     []={"白百摆拜败柏掰呗佰伯挀稗敗擺捭粺栢擘拝竡"};
//ban
//const unsigned char PY_mb_ban   []={"半办班般拌搬版斑板伴扳扮瓣颁绊癍坂钣舨阪"};
const unsigned char PY_mb_ban     []={"办版半班板般搬伴斑扮拌瓣绊扳颁坂阪辦钣瘢攽癍闆湴粄頒舨絆蝂柈姅昄鈑怑鉡靽螁魬斒"};
//bang
//const unsigned char PY_mb_bang  []={"帮棒绑磅镑邦榜蚌傍梆膀谤浜蒡"};
const unsigned char PY_mb_bang    []={"帮棒榜邦浜绑磅傍镑蚌谤膀幫梆幇垹綁旁蒡謗塝鞤鎊搒縍玤艕峀"};
//bao
//const unsigned char PY_mb_bao   []={"包抱报饱保暴薄宝爆剥豹刨雹褒堡苞胞鲍炮瀑龅孢煲褓鸨趵葆"};
const unsigned char PY_mb_bao     []={"包报宝抱保爆暴饱薄堡鲍煲鲍煲豹胞曝苞褒葆宀孢寶勹雹報菢寳刨飽剥鸨龅鮑儤褓瀑趵笣窇蕔珤媬鑤鴇虣炮齙怉靤駂鳵髱忁飹"};
//ben
//const unsigned char PY_mb_ben   []={"本奔苯笨夯锛贲畚坌"};
const unsigned char PY_mb_ben   []={"本笨奔苯锛贲渀犇畚坌夯錛泍栟"};
//beng
//const unsigned char PY_mb_beng  []={"蹦绷甭崩迸蚌泵甏嘣"};
const unsigned char PY_mb_beng  []={"泵蹦崩甭绷迸嘣蚌繃伻甏祊埲琫菶塴嵭閍"};
//bei
//const unsigned char PY_mb_bei   []={"被北倍杯背悲备碑卑贝辈钡焙狈惫臂褙悖蓓鹎鐾呗"};
const unsigned char PY_mb_bei   []={"被呗北背倍杯贝备悲碑卑辈蓓焙惫钡悖陂備孛唄狈臂貝碚盃輩偝珼邶鹎褙鞴琲牬鐾庳椑誖鋇揹鞁桮憊襬狽梖骳藣糒鄁犕"};
//bi
//const unsigned char PY_mb_bi    []={"比笔闭鼻碧必避逼毕臂彼鄙壁蓖币弊辟蔽毙庇敝陛毖痹秘泌秕薜荸芘萆匕裨畀俾嬖狴筚箅篦舭荜襞庳铋跸吡愎贲滗濞璧哔髀弼妣婢"};
const unsigned char PY_mb_bi    []={"比必笔逼币俾碧毕壁避鼻闭彼臂弊屄畀璧毙辟蔽弼痹敝庇鄙吡婢滗陛荜铋匕濞哔妣薜篦筆湢楅蓖芘嬖髀妼毖畢秕苾彃襞繴閉裨箅佖跸嬶泌愎筚幣堛萆斃躄睥贲閟荸疪詖狴柀賁粊庳獙沘邲痺嗶皕觱秘潷珌綼閇蹕匂舭縪愊幤怭鄪髲坒篳鏎獘廦腷贔蓽貏熚疕罼駜朼蜌奰鄨襣飶鵖鷩鼊梐馝鷝滭"};
//bian
//const unsigned char PY_mb_bian  []={"边变便遍编辩扁贬鞭卞辨辫忭砭匾汴碥蝙褊鳊笾苄窆弁缏"};
const unsigned char PY_mb_bian  []={"便边变遍编扁辩鞭辨鞭辨贬卞辫匾煸弁變苄邊汴編砭変釆褊笾忭辯蝙邉鳊貶抃箯碥缏窆籩辮髟辧萹揙艑豍惼甂峅汳稨鴘"};
//biao
//const unsigned char PY_mb_biao  []={"表标彪膘杓婊飑飙鳔瘭飚镳裱骠镖"};
const unsigned char PY_mb_biao  []={"表标彪飙飚镖膘裱婊骠镳杓灬標飑摽俵錶鳔飆儦諘藨褾驃鑣熛鏢瘭猋滮檦贆爂墂髟瀌臕飇鰾"};
//bie
//const unsigned char PY_mb_bie   []={"别憋鳖瘪蹩"};
const unsigned char PY_mb_bie   []={"别憋鳖瘪別蹩彆鱉咇蟞柲癟鼈虌襒徶蛂"};
//bin
//const unsigned char PY_mb_bin   []={"宾濒摈彬斌滨豳膑殡缤髌傧槟鬓镔玢"};
const unsigned char PY_mb_bin   []={"斌彬宾滨鬓镔濒殡缤槟殯摈玢膑豳髌賓傧儐邠濱鬢繽瀕蠙擯濵臏虨椕髕霦鑌"};
//bing
//const unsigned char PY_mb_bing  []={"并病兵冰丙饼屏秉柄炳摒槟禀邴"};
const unsigned char PY_mb_bing  []={"并病冰兵饼丙柄炳秉並禀氷摒幷邴屏餅槟併鞞昺偋掤抦稟垪倂棅餠竝寎梹檳苪誁陃窉怲庰鞆蛃鋲栟"};
//bo
//const unsigned char PY_mb_bo    []={"拨波播泊博伯驳玻剥薄勃菠钵搏脖帛般柏舶渤铂箔膊魄卜礴跛檗亳鹁踣啵蕃簸钹饽擘"};
const unsigned char PY_mb_bo    []={"波博播拨伯薄啵剥搏柏勃泊膊驳卜钵玻脖铂癶帛渤舶跛箔菠亳撥钹簸擘僰剝礴魄礡孛浡踣鎛愽檗缽饽駁鹁駮嶓蘗狛蔔詙簙譒僠鈸犦襮鉢糪秡懪欂瓝鋍葧胉鉑鑮餺蹳餑郣挬鱍鵓"};
//bu
//const unsigned char PY_mb_bu    []={"不步补布部捕卜簿哺堡埠怖埔瓿逋晡钸钚醭卟"};
const unsigned char PY_mb_bu    []={"不部补布步卜捕簿埠卟吥哺怖钚補逋歩堡佈埔瓿晡钸咘鸔鳪鈽餔醭蔀誧踄峬篰轐"};

/*** C ***/
//ca
const unsigned char PY_mb_ca    []={"擦拆嚓礤礸"};
//cai
//const unsigned char PY_mb_cai   []={"猜才材财裁采彩睬踩菜蔡"};
const unsigned char PY_mb_cai   []={"才菜猜踩蔡采彩财材裁睬財戝採婇偲綵棌寀埰跴縩"};
//can
//const unsigned char PY_mb_can   []={"蚕残掺参惨惭餐灿骖璨孱黪粲"};
const unsigned char PY_mb_can   []={"惨残餐参灿蚕惭璨粲殘慘參骖孱憯燦黪蠶慚飡驂慙澯黲嬠"};
//cang
//const unsigned char PY_mb_cang  []={"藏仓沧舱苍伧"};
const unsigned char PY_mb_cang  []={"藏仓苍舱沧倉蒼滄伧艙臧欌鑶獊嵢凔"};
//cao
//const unsigned char PY_mb_cao   []={"草操曹槽糙嘈艚螬漕"};
const unsigned char PY_mb_cao   []={"草操曹槽肏艹糙嘈漕嶆螬艸艚騲襙曺"};
//ce
//const unsigned char PY_mb_ce    []={"册侧策测厕恻"};
const unsigned char PY_mb_ce    []={"测侧册策厕恻側憡測冊廁惻萴蓛"};
//cen
//const unsigned char PY_mb_cen   []={"参岑涔"};
const unsigned char PY_mb_cen   []={"岑涔参梣嵾"};
//ceng
//const unsigned char PY_mb_ceng  []={"曾层蹭噌"};
const unsigned char PY_mb_ceng  []={"曾层蹭噌層嶒驓"};
//cha
//const unsigned char PY_mb_cha   []={"查插叉茶差岔搽察茬碴刹诧楂槎镲衩汊馇檫姹杈锸嚓"};
const unsigned char PY_mb_cha   []={"差查茶插叉察茬搽岔碴槎檫汊杈诧姹镲嚓刹衩楂偛侘锸鍤猹査馇垞喳嗏臿奼靫疀蹅艖紁"};
//chai
//const unsigned char PY_mb_chai  []={"菜柴拆差豺钗瘥虿侪"};
const unsigned char PY_mb_chai  []={"拆柴差钗豺犲侪虿瘥釵儕喍蠆祡袃囆"};
//chan
//const unsigned char PY_mb_chan  []={"产缠掺搀阐颤铲谗蝉单馋觇婵蒇谄冁廛孱蟾羼镡忏潺禅骣躔澶"};
const unsigned char PY_mb_chan  []={"产缠禅铲颤馋掺蝉婵谗搀蟾澶阐忏煘觇谄蒇潺躔禪鑱单廛產儳顫孱羼纏瀍鋋劖蟬襜巉辿摻懺瀺饞冁讒棎丳毚闡摲諂湹鏟骣嬋剷産灛嵼幨旵蕆壥纒繟梴懴簅滻囅嚵幝獑磛酁艬鋓"};
//chang
//const unsigned char PY_mb_chang []={"长唱常场厂尝肠畅昌敞倡偿猖裳鲳氅菖惝嫦徜鬯阊怅伥昶苌娼"};
const unsigned char PY_mb_chang []={"长唱常厂场昌尝肠畅偿娼倡敞長昶怅嫦場苌嘗猖徜阊廠鲳伥氅菖裳鬯暢償腸倘鋹嚐萇悵厰僘惝錩甞韔蟐椙膓淐裮琩鯧鋿鱨鼚"};
//chao
//const unsigned char PY_mb_chao  []={"朝抄超吵潮巢炒嘲剿绰钞怊焯耖晁"};
const unsigned char PY_mb_chao  []={"超炒朝吵抄潮巢钞绰嘲焯晁牊耖剿鈔怊弨巣漅眧鄛罺巐轈窲"};
//che
//const unsigned char PY_mb_che   []={"车撤扯掣彻尺澈坼砗"};
const unsigned char PY_mb_che   []={"车扯撤彻澈車掣坼徹偖砗尺迠硨硩撦烢莗"};
//chen
//const unsigned char PY_mb_chen  []={"趁称辰臣尘晨沉陈衬橙忱郴榇抻谌碜谶宸龀嗔伧琛"};
const unsigned char PY_mb_chen  []={"陈趁沉晨尘臣辰称衬琛嗔宸郴谌陳抻忱谶瞋碜塵伧龀榇傖襯讖棽疢沈蔯踸諃煁茞諶莐磣嚫贂墋螴肜鷐賝揨樄麎齔"};
//cheng
//const unsigned char PY_mb_cheng []={"成乘盛撑称城程呈诚惩逞骋澄橙承塍柽埕铖噌铛酲晟裎枨蛏丞瞠"};
const unsigned char PY_mb_cheng []={"成称城呈程乘撑诚承橙澄秤丞盛逞惩骋稱铖棖塍埕悜誠蛏瞠偁脭撐琤裎柽宬枨澂酲郕珹懲牚橕掁荿铛珵棦騁峸鋮庱檉碀蟶筬赬浾騬溗竀饓鏿"};
//chi
//const unsigned char PY_mb_chi   []={"吃尺迟池翅痴赤齿耻持斥侈弛驰炽匙踟坻茌墀饬媸豉褫敕哧瘛蚩啻鸱眵螭篪魑叱彳笞嗤"};
const unsigned char PY_mb_chi   []={"吃持池迟赤尺齿痴驰翅耻斥炽弛叺彳敕叱嗤螭匙豉侈哧墀饬湁笞篪呎蚩鸱茌遲啻魑媸黐褫傺歯瘛恥齒馳絺坻摛癡熾勅懘喫瘈踟鶒眵鉓妛荎抶攡淔痸遅鴟遫蚇瞝謘漦瓻鉹胣鷘翨鵄腟貾饎岻齝麶"};
//chou
//const unsigned char PY_mb_chou  []={"抽愁臭仇丑稠绸酬筹踌畴瞅惆俦帱瘳雠"};
const unsigned char PY_mb_chou  []={"抽丑臭愁仇筹畴瞅稠酬绸俦踌醜雠惆瘳籌栦疇讎篘吜犨綢菗絒帱殠讐杻杽雔燽躊懤嬦"};
//chong
//const unsigned char PY_mb_chong []={"冲重虫充宠崇涌种艟忡舂铳憧茺"};
const unsigned char PY_mb_chong []={"冲重充虫宠崇翀铳舂衝沖憧忡蟲蹖寵种艟茺銃珫罿隀浺憃"};
//chu
//const unsigned char PY_mb_chu   []={"出处初锄除触橱楚础储畜滁矗搐躇厨雏楮杵刍怵绌亍憷蹰黜蜍樗"};
const unsigned char PY_mb_chu   []={"出处除初楚触储厨褚畜雏锄杵橱亍処處怵矗刍础滁黜楮蹰濋绌樗俶躇滀搐儊儲絀齣敊憷觸芻廚貙雛櫥摴蜍歜篨蒢鋤斶鶵耡豖礎趎懨犓蒭幮鄐珿臅躕檚琡"};
//chua
const unsigned char PY_mb_chua []={"欻"};
//chuai
//const unsigned char PY_mb_chuai []={"揣膪啜嘬搋踹"};
const unsigned char PY_mb_chuai []={"踹揣啜搋嘬膪欼膗"};
//chuan
//const unsigned char PY_mb_chuan []={"穿船传串川喘椽氚遄钏舡舛巛"};
const unsigned char PY_mb_chuan []={"传穿船川串喘傳舛椽钏巛氚荈遄舡踳瑏釧汌歂賗玔輲暷"};
//chuang
//const unsigned char PY_mb_chuang[]={"窗床闯创疮幢怆"};
const unsigned char PY_mb_chuang[]={"床创窗闯疮怆創幢闖摐瘡傸愴牀剏刱窓磢"};
//chui
//const unsigned char PY_mb_chui  []={"吹垂炊锤捶椎槌棰陲"};
const unsigned char PY_mb_chui  []={"吹垂锤捶炊槌棰陲缍圌椎錘鎚菙"};
//chun
//const unsigned char PY_mb_chun  []={"春唇纯蠢醇淳椿蝽莼鹑"};
const unsigned char PY_mb_chun  []={"春纯唇蠢醇淳椿純莼蝽鹑脣偆賰暙橁蓴犉惷漘杶萶鶉瑃鰆槆媋鶞鯙"};
//chuo
//const unsigned char PY_mb_chuo  []={"戳绰踔啜龊辍"};
const unsigned char PY_mb_chuo  []={"戳绰啜辍龊踔磭綽歠娖輟惙逴齪嚽擉"};
//ci
//const unsigned char PY_mb_ci    []={"次此词瓷慈雌磁辞刺茨伺疵赐差兹呲鹚祠糍"};
const unsigned char PY_mb_ci    []={"次此词刺辞赐磁瓷慈茨雌祠兹疵差詞糍佽鹚賜栨辭伺泚偨佌茈嬨絘庛朿跐蛓柌莿濨趀飺餈鶿皉茦"};
//cong
//const unsigned char PY_mb_cong  []={"从丛葱匆聪囱琮枞淙璁骢苁"};
const unsigned char PY_mb_cong  []={"从葱聪丛從琮璁匆骢淙熜婃囱聰従枞苁蔥囪悰聡叢怱悤賨驄孮慒謥暰欉鏦藂蟌樅瑽瞛徖"};
//cou
//const unsigned char PY_mb_cou   []={"凑楱辏腠"};
const unsigned char PY_mb_cou   []={"凑腠湊辏玼楱輳"};
//cu
//const unsigned char PY_mb_cu    []={"粗醋簇促卒徂猝蔟蹙酢殂蹴"};
const unsigned char PY_mb_cu    []={"粗促醋簇蹙蹴猝徂蔟殂卒瘯憱酢麁顣麤觕踧踀鼀"};
//cuan
//const unsigned char PY_mb_cuan  []={"窜蹿篡攒撺汆爨镩"};
const unsigned char PY_mb_cuan  []={"窜蹿攒篡爨汆撺竄攢镩攅巑攛躥簒"};
//cui
//const unsigned char PY_mb_cui   []={"催脆摧翠崔淬衰瘁粹璀啐悴萃毳榱"};
const unsigned char PY_mb_cui   []={"催崔翠脆摧粹萃淬啐璀瘁悴毳衰榱漼墔膵凗焠膬磪忰翆鏙竁慛"};
//cun
//const unsigned char PY_mb_cun   []={"村寸存蹲忖皴"};
const unsigned char PY_mb_cun   []={"村存寸吋忖皴邨蹲籿刌"};
//cuo
//const unsigned char PY_mb_cuo   []={"错撮搓挫措磋嵯厝鹾脞痤蹉瘥锉矬"};
const unsigned char PY_mb_cuo   []={"错撮搓措挫厝錯锉磋矬痤鹾蹉夎嵯棤剉脞瘥齹剒瑳醝鹺莝銼蒫蓌遳嵳髊"};

/*** D ***/
//da
//const unsigned char PY_mb_da    []={"大答达打搭瘩塔笪耷哒褡疸怛靼妲沓嗒鞑"};
const unsigned char PY_mb_da    []={"打大达答搭哒嗒妲怛達沓鞑耷笪瘩噠龘褡靼咑炟躂鐽薘燵羍繨韃匒鎝畣"};
//dai
//const unsigned char PY_mb_dai   []={"带代呆戴待袋逮歹贷怠傣大殆呔玳迨岱甙黛骀绐埭"};
const unsigned char PY_mb_dai   []={"带待代呆戴袋贷黛逮岱大甙歹帶怠殆傣埭玳迨绐呔汏帯貸瀻軑襶柋紿獃靆軩艜"};
//dan
//const unsigned char PY_mb_dan   []={"但单蛋担弹掸胆淡丹耽旦氮诞郸惮石疸澹瘅萏殚眈聃箪赕儋啖"};
const unsigned char PY_mb_dan   []={"但单蛋丹淡胆担弹氮旦诞啖耽卩掸澹儋單惮殚亶勯箪聃膽窞眈噉擔帎赕石郸紞疸殫誕啗泹啿憚憺簞褝亻萏瘅柦襌酖蜑玬譂觛禫匰燀狚嚪砃甔聸贉癚鄲澸刐髧沊鴠黕霮衴"};
//dang
//const unsigned char PY_mb_dang  []={"当党挡档荡谠铛宕菪凼裆砀"};
const unsigned char PY_mb_dang  []={"当党档挡荡當裆铛宕檔凼黨砀谠噹擋儅蕩菪璫簹讜盪鐺襠璗礑蟷澢碭艡瓽"};
//dao
//const unsigned char PY_mb_dao   []={"到道倒刀岛盗稻捣悼导蹈祷帱纛忉焘氘叨"};
const unsigned char PY_mb_dao   []={"到道倒刀岛导盗稻捣悼屶槝叨蹈刂祷島纛忉瓙氘菿導嶋盜檤稲儔魛椡捯焘禱裯搗翿嶌帱舠禂擣壔嶹幬釖隝陦隯"};
//de
//const unsigned char PY_mb_de    []={"的地得德底锝"};
const unsigned char PY_mb_de    []={"的得地德锝底徳悳淂"};
//den
const unsigned char PY_mb_den    []={"扽扥"};
//deng
//const unsigned char PY_mb_deng  []={"等灯邓登澄瞪凳蹬磴镫噔嶝戥簦"};
const unsigned char PY_mb_deng  []={"等灯登邓瞪蹬凳噔澄镫燈磴鄧鐙嶝戥簦豋墱嬁璒"};
//dei
const unsigned char PY_mb_dei    []={"得"};
//di
//const unsigned char PY_mb_di    []={"地第底低敌抵滴帝递嫡弟缔堤的涤提笛迪狄翟蒂觌邸谛诋嘀柢骶羝氐棣睇娣荻碲镝坻籴砥"};
const unsigned char PY_mb_di    []={"地第低滴底睇帝的迪弟敌抵递蒂狄堤笛谛娣嘀荻涤邸旳骶棣嫡缔氐菂砥镝觌翟诋碲柢籴敵俤聜墬鞮袛坻磾諦奀滌遞提厎羝禘牴玓蚳締墑苖螮踶糴彽菧嚁蔕阺呧廸隄釱蔋鏑馰藡鸐逓梑髢眱軧靮蹢焍弤甋梊"};
//dia
const unsigned char PY_mb_dia	[]={"嗲"};
//dian
//const unsigned char PY_mb_dian  []={"点电店殿淀掂颠垫碘惦奠典佃靛滇甸踮钿坫阽癫簟玷巅癜"};
const unsigned char PY_mb_dian  []={"点店电殿垫典颠掂碘滇甸點癫丶踮靛淀巅奠佃癜簟钿惦玷電敁槙坫顛厧傎磹墊癲瘨扂巔蒧壂婰巓蹎阽椣澱蕇驔齻婝"};
//diao
//const unsigned char PY_mb_diao  []={"掉钓叼吊雕调刁碉凋鸟铞铫鲷貂"};
const unsigned char PY_mb_diao  []={"掉吊调叼钓雕屌刁凋貂弔鲷碉鸟铞釣弴伄魡蛁鯛铫琱彫蓧鵰虭殦雿簓奝窵"};
//die
//const unsigned char PY_mb_die   []={"爹跌叠碟蝶迭谍牒堞瓞揲蹀耋鲽垤喋"};
const unsigned char PY_mb_die   []={"跌碟爹蝶叠迭谍牒褋喋耋堞鲽垤疊蹀諜瓞疂昳詄咥峌絰艓胅嵽蜨畳疉眣镻苵跮"};
//ding
//const unsigned char PY_mb_ding  []={"顶定盯订叮丁钉鼎锭町玎铤腚碇疔仃耵酊啶"};
const unsigned char PY_mb_ding  []={"定顶丁订盯鼎钉锭叮町腚頂啶碇酊玎仃釘訂疔耵萣掟錠帄嵿顁椗虰聢飣靪薡碠甼"};
//diu
//const unsigned char PY_mb_diu   []={"丢铥"};
const unsigned char PY_mb_diu   []={"丢丟铥颩銩"};
//dong
//const unsigned char PY_mb_dong  []={"动东懂洞冻冬董栋侗恫峒鸫垌胨胴硐氡岽咚"};
const unsigned char PY_mb_dong  []={"动懂东洞董冬栋冻咚氡東動侗夂峒硐恫胴棟垌鸫凍岽戙胨崠姛蝀働諌涷苳挏嬞迵蕫鮗揰菄埬鼕鶇霘駧鶫"};
//dou
//const unsigned char PY_mb_dou   []={"都斗豆逗陡抖痘兜读蚪窦篼蔸"};
const unsigned char PY_mb_dou   []={"都斗豆逗抖兜窦陡痘蔸脰鬥钭枓篼闘蚪竇鈄荳鬪读郖唗酘浢餖梪瞗"};
//du
//const unsigned char PY_mb_du    []={"读度毒渡堵独肚镀赌睹杜督都犊妒顿蠹笃嘟渎椟牍黩髑芏"};
const unsigned char PY_mb_du    []={"读度都毒杜独赌堵渡肚督嘟镀睹笃妒犊渎讀蠹獨顿殰牍賭牘篤椟芏涜黩読鍍秺髑瀆覩妬犢帾蝳櫝讟詫蠧贕韥皾韣裻黷醏韇錖"};
//duan
//const unsigned char PY_mb_duan  []={"段短断端锻缎椴煅簖"};
const unsigned char PY_mb_duan  []={"段短断端锻缎椴煅斷鍛緞偳簖葮毈腶籪媏躖碫"};
//dui
//const unsigned char PY_mb_dui   []={"对队堆兑敦镦碓怼憝"};
const unsigned char PY_mb_dui   []={"对队堆對兑碓隊対敦怼兌镦憝搥懟濧薱譈瀩鴭痽祋頧"};
//dun
//const unsigned char PY_mb_dun   []={"吨顿蹲墩敦钝盾囤遁不趸沌盹镦礅炖砘"};
const unsigned char PY_mb_dun   []={"吨顿盾炖蹲敦墩钝遁盹趸囤沌惇頓鐓礅伅噸燉砘潡楯不鈍遯躉镦撉壿驐"};
//duo
//const unsigned char PY_mb_duo   []={"多朵夺舵剁垛跺惰堕掇哆驮度躲踱沲咄铎裰哚缍"};
const unsigned char PY_mb_duo   []={"多躲夺朵堕剁哆铎跺踱舵垛剟惰掇哚敓埵咄柁奪墮朶驮躱沲裰度墯跢鐸錞杕柮陊剫痑嚲敪奲趓嶞毲綞鈬椯鮵鬌鵽"};

/*** E ***/
//e
//const unsigned char PY_mb_e     []={"饿哦额鹅蛾扼俄讹阿遏峨娥恶厄鄂锇谔垩锷阏萼苊轭婀莪鳄颚腭愕呃噩鹗屙"};
const unsigned char PY_mb_e     []={"额俄饿呃恶哦厄诶鹅鄂娥莪蛾扼峨噩鳄屙丨萼颚讹峩遏愕腭锷谔鍔枙惡皒轭涐婀餓垩阿額鹗欸锇噁鵝砵苊訛顎囮搹迗娾珴悪阨崿鱷睋諤譌鵞頞咢軛堮遻砐廅堊鑩蝁峉鰐鶚歞軶匎餩齶蚅蕚砨鵈櫮豟"};
//en
//const unsigned char PY_mb_en    []={"恩摁嗯"};
const unsigned char PY_mb_en    []={"恩嗯唔摁蒽奀峎"};
//eng
const unsigned char PY_mb_eng    []={"鞥"};
//ei
const unsigned char PY_mb_ei    []={"诶"};
//er
//const unsigned char PY_mb_er    []={"而二耳儿饵尔贰洱珥鲕鸸佴迩铒"};
const unsigned char PY_mb_er    []={"而二儿尔耳贰饵迩弍兒洱珥铒爾鸸洏栭貳侕餌鲕佴鉺邇弐毦陑咡貮粫荋刵薾栮胹輀峏鴯駬鮞轜衈"};

/*** F ***/
//fa
//const unsigned char PY_mb_fa    []=	 {"发法玥罚伐乏筏阀珐垡砝"};
const unsigned char PY_mb_fa    []=	 {"发法伐罚阀乏發筏珐髮垡発罰髪砝閥琺沷醗瞂罸疺藅"};
//fan
//const unsigned char PY_mb_fan   []={"反饭翻番犯凡帆返泛繁烦贩范樊藩矾钒燔蘩畈蕃蹯梵幡"};
const unsigned char PY_mb_fan   []={"饭反烦翻凡范返犯番泛繁樊帆贩梵藩钒蕃幡攵矾勫飯畈犭煩璠蘩燔範墦辺販仮柉汎氾蹯繙旛薠瀿籵笵轓軓膰礬奿釩杋鐇飜瀪払軬蠜笲羳嬏舩旙颿橎滼鱕鷭"};
//fang
//const unsigned char PY_mb_fang  []={"放房防纺芳方访仿坊妨肪钫彷邡枋舫鲂"};
const unsigned char PY_mb_fang  []={"放方房防芳坊仿访纺舫妨枋昉肪鲂彷倣訪钫紡邡鈁昞魴瓬淓錺髣鶭"};
//fei
//const unsigned char PY_mb_fei   []={"非飞肥费肺废匪吠沸菲诽啡篚蜚腓扉妃斐狒芾悱镄霏翡榧淝鲱绯痱"};
const unsigned char PY_mb_fei   []={"非飞费肥菲废肺妃匪斐绯沸翡霏吠扉啡飛鐨腓诽費痱俷淝蜚芾狒镄篚廢悱榧棐緋櫠馡怫癈剕婓鲱誹屝暃厞騑廃蜰裶騛奜蟦曊襏鯡濷餥昲疿胇鼣"};
//fen
//const unsigned char PY_mb_fen   []={"分份芬粉坟奋愤纷忿粪酚焚吩氛汾棼瀵鲼玢偾鼢"};
const unsigned char PY_mb_fen   []={"分份粉芬粪坟奋酚愤纷焚汾忿氛鼢瀵吩濆鲼偾玢墳蚡棻棼紛昐糞憤蒶奮枌僨蕡妢雰鈖羵坋黺豶翂轒魵弅膹幩橨梤鳻羒鼖炃黂饙竕鐼秎"};
//feng
//const unsigned char PY_mb_feng  []={"风封逢缝蜂丰枫疯冯奉讽凤峰锋烽砜俸酆葑沣唪"};
const unsigned char PY_mb_feng  []={"风封峰疯冯凤丰锋枫逢缝奉蜂烽沣讽風俸砜鳳葑瘋賵酆楓豐鋒馮偑崶峯縫諷灃唪檒渢犎覂堸妦桻猦麷凬捀甮焨艂飌"};
//fiao
const unsigned char PY_mb_fiao    []={"覅"};
//fo
//const unsigned char PY_mb_fo    []={"佛"};
const unsigned char PY_mb_fo    []={"佛仏坲"};
//fou
//const unsigned char PY_mb_fou   []={"否缶"};
const unsigned char PY_mb_fou   []={"否缶紑缹鴀"};
//fu
//const unsigned char PY_mb_fu    []={"副幅扶浮富福负伏付复服附俯斧赴缚拂夫父符孵敷赋辅府腐腹妇抚覆辐肤氟佛俘傅讣弗涪袱市甫釜脯腑阜咐黼砩苻趺跗蚨芾鲋幞茯滏蜉拊菔蝠鳆蝮绂绋赙罘稃匐麸凫桴莩孚馥驸怫祓呋郛芙艴"};
const unsigned char PY_mb_fu    []={"副付服福富负夫复赴府父附符傅扶浮幅腹伏敷赋弗甫妇覆辅抚腐拂氟芙肤斧阜俯缚孚馥佛釜孵腑辐俘蝠凫苻涪祓脯麸復呋頫讣褔冨黻滏芣罘咐鍑赙拊輔黼洑賦匐绂绋負蝮茀驸鲋跗鵩桴茯撫袱趺婦幞諨綍伕鄜膚稃複郛縛砩菔偩姇艴祔帗芾簠枹駙蚨蜉鈇冹偪怫袚紱輻紨彿箙鳆枎蝜莩柎泭衭垘榑咈緮荂刜虙砆麩坿訃琈綒輹蚹岪鮒椨萯鳧柭葍怤笰紼罦萉賻釡蚥澓蕧髴弣烰鰒蜅郙踾玸荴韍鶝柫鳬艀邞嬔蛗畉糐翇鴔"};


/*** G ***/
//ga
//const unsigned char PY_mb_ga    []={"噶嘎咖伽尬尕轧旮钆夹尜"};
const unsigned char PY_mb_ga    []={"噶胳夹嘎咖轧钆伽旮尬尕尜呷"};
//gai
//const unsigned char PY_mb_gai   []={"该改盖概钙芥溉戤垓丐陔赅"};
const unsigned char PY_mb_gai   []={"该改盖钙概丐乢該蓋溉赅垓陔芥鈣祴荄葢戤瓂槪賅漑豥峐賌"};
//gan
//const unsigned char PY_mb_gan   []={"赶干感敢竿甘肝柑杆赣秆旰酐矸疳泔苷擀绀橄澉淦尴坩"};
const unsigned char PY_mb_gan   []={"干甘感敢赶肝杆赣竿苷乾柑咁秆淦擀幹绀橄酐矸趕疳尴坩皯泔詌桿旰玕澉紺簳贛稈笴凎筸骭玵尷盰灨漧"};
//gang
//const unsigned char PY_mb_gang  []={"刚钢纲港缸岗杠冈肛扛筻罡戆"};
const unsigned char PY_mb_gang  []={"刚港钢岗缸纲冈杠肛罡戆剛崗岡鋼扛綱槓釭棡筻堽犅堈疘"};
//gao
//const unsigned char PY_mb_gao   []={"高搞告稿膏篙羔糕镐皋郜诰杲缟睾槔锆槁藁"};
const unsigned char PY_mb_gao   []={"高搞告稿膏糕皋镐锆郜羔篙诰杲睾缟槁藁睪髙檺槔誥皐槹縞櫜韟祰鼛鋯勂筶鷎"};
//ge
//const unsigned char PY_mb_ge    []={"个各歌割哥搁格阁隔革咯胳葛蛤戈鸽疙盖屹合铬硌骼颌袼塥虼圪镉仡舸鬲嗝膈搿纥哿"};
const unsigned char PY_mb_ge    []={"个各格歌哥隔割阁葛戈咯搁革個铬鸽镉舸嗝膈戓硌疙圪胳閣鬲彁骼纥颌箇滆謌哿搿擱盖佮虼閤袼蛤獦鉻仡鴿轕塥滒鎘觡合韐吤愅鮯杚鞷鴐騔"};
//gei
//const unsigned char PY_mb_gei   []={"给"};
const unsigned char PY_mb_gei   []={"给給"};
//gen
//const unsigned char PY_mb_gen   []={"跟根哏茛亘艮"};
const unsigned char PY_mb_gen   []={"跟根艮哏亘茛揯"};
//geng
//const unsigned char PY_mb_geng  []={"更耕颈梗耿庚羹埂赓鲠哽绠"};
const unsigned char PY_mb_geng  []={"更庚耿耕梗羹哽埂鲠赓绠颈綆亙畊浭挭緪賡鯁堩郠峺骾菮鶊"};
//gong
//const unsigned char PY_mb_gong  []={"工公功共弓攻宫供恭拱贡躬巩汞龚红肱觥珙蚣"};
const unsigned char PY_mb_gong  []={"共工公供宫功攻弓龚拱恭贡巩汞躬宮肱觥珙蚣貢栱幊塨鞏红龔拲輁匑熕"};
//gou
//const unsigned char PY_mb_gou   []={"够沟狗钩勾购构苟垢句岣彀枸鞲觏缑笱诟遘媾篝佝"};
const unsigned char PY_mb_gou   []={"够狗购沟勾钩构苟垢夠媾诟篝枸觏彀岣缑遘姤溝鉤佝笱緱鞲購芶袧鈎構簼冓搆耇雊覯韝耉玽句"};
//gu
//const unsigned char PY_mb_gu    []={"古股鼓谷故孤箍姑顾固雇估咕骨辜沽蛊贾菇梏鸪汩轱崮菰鹄鹘钴臌酤呱鲴诂牯瞽毂锢牿痼觚蛄罟嘏"};
const unsigned char PY_mb_gu    []={"股故古顾谷骨鼓孤固姑估雇沽菇蛊箍辜钴呱咕瞽毂崮菰臌牯鹄顧罛痼诂锢觚蛄汩轱榖穀酤罟箛棝鹘鲴鸪蠱梏贾稒鈷嘏僱牿羖詁堌盬榾愲柧轂泒凅皷峠軱尳嫴啒蓇鴣錮蛌瀔硲鮕唃薣濲橭逧"};
//gua
//const unsigned char PY_mb_gua   []={"挂刮瓜寡剐褂卦呱胍鸹栝诖"};
const unsigned char PY_mb_gua   []={"挂瓜刮卦寡呱剐褂胍栝掛诖鸹罣剮苽劀緺騧括詿颪鴰颳"};
//guai
//const unsigned char PY_mb_guai  []={"怪拐乖掴"};
const unsigned char PY_mb_guai  []={"乖怪拐夬恠掴柺枴罫"};
//guan
//const unsigned char PY_mb_guan  []={"关管官观馆惯罐灌冠贯棺纶盥矜莞掼涫鳏鹳倌"};
const unsigned char PY_mb_guan  []={"管关官观馆灌冠罐惯贯棺莞倌關祼觀掼館鹳琯瓘纶鳏鏆涫盥観矜慣貫瘝摜閞悹錧鑵舘筦蒄悺毌爟鸛樌鱹鱞痯"};
//guang
//const unsigned char PY_mb_guang []={"光广逛桄犷咣胱"};
const unsigned char PY_mb_guang []={"光广逛咣廣桄胱犷洸広侊垙俇撗珖銧茪欟烡炚"};
//gui
//const unsigned char PY_mb_gui   []={"归贵鬼跪轨规硅桂柜龟诡闺瑰圭刽傀癸炔庋宄桧刿鳜鲑皈匦妫晷簋炅"};
const unsigned char PY_mb_gui   []={"贵鬼归桂规龟柜硅跪轨圭闺癸诡瑰珪妫貴櫃桧佹歸鲑刿簋炅皈匦晷規龜垝姽庋槻湀槼宄劌亀鳜刽邽瞡軌詭袿帰媯摫庪閨匭劊槶厬撌氿蟡觤椢鳺禬蛫祪筀茥猤"};
//gun
//const unsigned char PY_mb_gun   []={"滚棍辊鲧衮磙绲"};
const unsigned char PY_mb_gun   []={"滚棍辊衮鲧丨滾绲棞磙惃輥袞鯀璭蔉"};
//guo
//const unsigned char PY_mb_guo   []={"过国果裹锅郭涡埚椁聒馘猓崞掴帼呙虢蜾蝈"};
const unsigned char PY_mb_guo   []={"过国果郭锅裹過掴國囗虢椁鍋蝈帼呙粿聒聝涡淉埚幗菓馘惈崞蜾彉鐹猓摑瘑墎槨圀嘓褁慖蔮漍簂"};

/*** H ***/
const unsigned char PY_mb_ha[]={"哈蛤铪虾"};
//hai
//const unsigned char PY_mb_hai   []={"还海害咳氦孩骇骸亥嗨醢胲"};
const unsigned char PY_mb_hai   []={"还海害嗨咳孩還亥骇嘿氦骸醢嗐駭侅胲烸絯塰駴餀"};
//han
//const unsigned char PY_mb_han   []={"喊含汗寒汉旱酣韩焊涵函憨翰罕撼捍憾悍邯邗菡撖瀚阚顸蚶焓颔晗鼾"};
const unsigned char PY_mb_han   []={"汗喊含寒韩汉涵函焊翰旱憨瀚罕撼悍憾晗酣菡捍琀邯漢焓韓鼾颔邗蚶顸扞犴晥榦撖阚螒頇唅豃閈釬暵凾虷谽哻蔊魽鋎頷銲鋡駻涆蜬顄睅鶾嵅甝鬫梒蜭雗莟"};
//hang
//const unsigned char PY_mb_hang  []={"行巷航夯杭吭颃沆绗珩"};
const unsigned char PY_mb_hang  []={"行航杭夯吭巷绗沆垳斻颃迒苀貥笐魧蚢絎頏"};
//hao
//const unsigned char PY_mb_hao   []={"好号浩嚎壕郝毫豪耗貉镐昊颢灏嚆蚝嗥皓蒿濠薅"};
const unsigned char PY_mb_hao   []={"好号浩豪耗郝昊皓毫灏嚎蒿濠蚝號壕颢镐嗥薅貉鎬嚆滈淏灝皜皞澔顥蠔晧暠哠鄗譹籇薃昦竓鰝"};
//he
//const unsigned char PY_mb_he    []={"和喝合河禾核何呵荷贺赫褐盒鹤菏貉阂涸吓嗬劾盍翮阖颌壑诃纥曷"};
const unsigned char PY_mb_he    []={"和喝何合河呵核盒贺荷鹤赫禾褐诃壑颌阖涸爀嗬阂菏犵翮劾曷貉訶楁賀盍鶴鞨啝郃纥敆吓姀蚵佫粭挌闔翯鉌盉熆熇澕龢覈渮煂寉紇鶡魺閡靍齕峆靏籺篕餲鑉齃麧"};
//hei
//const unsigned char PY_mb_hei   []={"黑嘿"};
const unsigned char PY_mb_hei   []={"黑嘿嗨黒潶"};
//hen
//const unsigned char PY_mb_hen   []={"很狠恨痕"};
const unsigned char PY_mb_hen   []={"很恨狠痕鞎拫"};
//heng
//const unsigned char PY_mb_heng  []={"横恒哼衡亨行桁珩蘅"};
const unsigned char PY_mb_heng  []={"横哼恒衡亨珩蘅佷桁橫姮恆脝烆啈鑅胻鵆"};
//hong
//const unsigned char PY_mb_hong  []={"红轰哄虹洪宏烘鸿弘讧訇蕻闳薨黉荭泓"};
const unsigned char PY_mb_hong  []={"红宏洪哄鸿虹弘轰泓烘紅薨闳吽紘荭苰訇竑鴻鍧黉仜轟垬蕻魟愩鋐翃葒讧鉷浤澒澋汯灴渱訌玒嗊谹谼吰彋竤鈜軣嚝閎霐霟妅鞃輷耾黌羾焢篊宖"};
//hou
//const unsigned char PY_mb_hou   []={"后厚吼喉侯候猴鲎篌堠後逅糇骺瘊"};
const unsigned char PY_mb_hou   []={"后厚後侯吼候猴喉鲎骺篌逅堠垕瘊齁洉糇詬葔腄郈帿鍭翭茩鯸鄇鱟餱"};
//hu
//const unsigned char PY_mb_hu    []={"湖户呼虎壶互胡护糊弧忽狐蝴葫沪乎戏核和瑚唬鹕冱怙鹱笏戽扈鹘浒祜醐琥囫烀轷瓠煳斛鹄猢惚岵滹觳唿槲"};
const unsigned char PY_mb_hu    []={"胡户湖虎呼沪乎互护忽糊壶狐弧唬寣弖扈斛鍙祜琥许浒笏瑚戏蝴葫瓠滹鹄和惚怙槲煳護戸戽壺戶唿觳綔乕鹘囫鹕嫭俿烀滬嫮猢枑縠醐岵鹱鄠冴粐核虖冱滸轷鵠淴鰗淈縎箎沍熩垀鬍楜抇瀫淲槴匢雽鯱曶媩歑螜昈昒鶻頀焀餬韄壷嘝鶦蔰婟鶘衚魱"};
//hng
const unsigned char PY_mb_hng   []={"哼"};
//hua
//const unsigned char PY_mb_hua   []={"话花化画华划滑哗猾豁铧桦骅砉"};
const unsigned char PY_mb_hua   []={"话花化画华划滑桦哗骅話畫華嬅铧樺猾畵錵埖劃糀椛槬砉嘩嫿螖搳崋譁杹吪繣驊摦諙魤澅硴鷨鏵蘤"};
//huai
//const unsigned char PY_mb_huai  []={"坏怀淮槐徊划踝"};
const unsigned char PY_mb_huai  []={"坏怀淮槐踝徊壞懷壊懐划褢蘾櫰褱蘹"};
//huan
//const unsigned char PY_mb_huan  []={"换还唤环患缓欢幻宦涣焕豢桓痪漶獾擐逭鲩郇鬟寰奂锾圜洹萑缳浣"};
const unsigned char PY_mb_huan  []={"换还环患欢幻缓唤焕桓浣寰宦換涣奂鬟獾洹嬛豢痪歡郇環缳擐圜喚驩鲩煥逭緩漶萑锾讙轘歓澣懽雚貆睆潅渙澴奐瘓峘豩荁犿鍰鴅繯豲嚾瑍闤羦攌肒鯇瞣鰀雈"};
//huang
//const unsigned char PY_mb_huang []={"黄慌晃荒簧凰皇谎惶蝗磺恍煌幌隍肓潢篁徨鳇"};
const unsigned char PY_mb_huang []={"黄晃皇慌荒煌谎凰恍簧磺璜黃惶蝗篁幌徨湟肓潢鍠愰隍遑偟怳崲滉癀皝喤晄謊汻蟥鳇巟媓穔獚兤熿艎諻榥衁鐄鰉炾堭趪楻鎤餭葟奛騜鷬"};
//hui
//const unsigned char PY_mb_hui   []={"回会灰绘挥汇辉毁悔惠晦徽恢秽慧贿蛔讳徊卉烩诲彗浍珲蕙喙恚哕晖隳麾诙蟪茴洄咴虺荟缋"};
const unsigned char PY_mb_hui   []={"会回灰辉汇惠慧毁挥會晖绘徽悔蕙卉秽晦讳恢烩诲囘荟喙贿茴彗拻恚珲洄麾桧輝隳蛔恵咴繪虺絵毀溃迴匯揮诙廻蟪佪諱徊缋穢暉誨彙恛撝薈槥鐬徻洃檜襘燴詼賄嘒輠燬翬顪篲硊毇譓橞螝圚潓薉烠噅蔧闠譿獩豗禈痐憓蘳芔暳譭檓楎瞺繢翽鰴隓餯藱檅"};
//hun
//const unsigned char PY_mb_hun   []={"混昏荤浑婚魂阍珲馄溷诨"};
const unsigned char PY_mb_hun   []={"混昏魂婚荤浑溷馄棔阍閽诨掍惛珲倱渾圂諢琿涽慁緄觨睯餛繉睧鼲"};
//huo
//const unsigned char PY_mb_huo   []={"或活火伙货和获祸豁霍惑嚯镬耠劐藿攉锪蠖钬夥"};
const unsigned char PY_mb_huo   []={"或活货火获霍祸惑伙豁和灬镬嚯佸獲鈥貨嚄禍藿钬豰夥耠蠖捇漷吙鑊攉劐湱锪濩秮臛姡咊謋膕曤嗀矐騞癨穫沎檴趏眓瀖"};

/*** J ***/
//ji
//const unsigned char PY_mb_ji    []={"几及急既即机鸡积记级极计挤己季寄纪系基激吉脊际击圾畸箕肌饥迹讥姬绩棘辑籍缉集疾汲嫉蓟技冀伎祭剂悸济寂期其奇忌齐妓继给革稽墼洎鲚屐齑戟鲫嵇矶稷戢虮诘笈暨笄剞叽蒺跻嵴掎跽霁唧畿荠瘠玑羁丌偈芨佶赍楫髻咭蕺觊麂"};
const unsigned char PY_mb_ji    []={"及级即几机既急集记极鸡寄基继吉计挤季剂暨纪己积籍技击姬济肌忌祭疾激迹辑际冀叽寂妓脊饥霁戟骥藉绩汲棘旡髻矶彑蓟亼畸伎箕丌唧讥畿稽系丮偈羁記機嫉極稷缉嵇幾鲫亟計悸玑屐級赍銈觊殛期笈紀其咭麂嵴奇戢楫剤跻雞繼佶圾给鐖骑笄瘠劑剞喞擠芨洎勣漈積撃績芰輯櫅乩擊掎犱齑濟革伋虮岌罽荠吇墼姞鶏蕺跡岋忣際偮茍蠀彶潗緝茤磯鲚诘譏跽卽嘰犄饑継蒺妀誋旣暩湒惎繋羈薺薊飢漃泲驥諅徛穄鷄蕀虀覿枅塈鶺霽諔羇璣皀叝躋蹟覊鞿齎蹐觙鮆撠覬揤覘臮簊踖膌鸄堲簎蘻隮齌谻嶯禝鱭裚瀱鰶槉穊齏鏶懻痵庴轚檕蘮錤癠銡霵刉癪鑇耭畟襋鱀韲螏鰿穖鷑鵋橶塉鯚"};
//jia
//const unsigned char PY_mb_jia   []={"家加假价架甲佳夹嘉驾嫁枷荚颊钾稼茄贾铗葭迦戛浃镓痂恝岬跏嘏伽胛笳珈瘕郏袈蛱"};
const unsigned char PY_mb_jia   []={"家加假价架甲佳夹嘉嫁贾驾钾迦茄颊稼痂珈葭荚枷伽戛镓岬戞笳鎵傢铗郏夾價瘕駕賈浃胛袈恝榎幏跏蛱嘏斝頰豭筴泇鉀莢耞糘頬鋏貑犌婽郟毠椵抸猳梜扴鵊玾檟蛺鴶麚"};
//jian
//const unsigned char PY_mb_jian  []={"见件减尖间键贱肩兼建检箭煎简剪歼监坚奸健艰荐剑渐溅涧鉴浅践捡柬笺俭碱硷拣舰槛缄茧饯翦鞯戋谏牮枧腱趼缣搛戬毽菅鲣笕谫楗囝蹇裥踺睑謇鹣蒹僭锏湔"};
const unsigned char PY_mb_jian  []={"见间建件剑键兼减贱健捡剪简箭检肩尖渐坚煎监舰拣奸碱鉴溅荐涧歼茧俭谏践笺見柬艰牋睑戋翦蹇間腱僭劍菅缄熸槛簡硷饯幵锏鑳賤繝犍囝戬谫蒹湔剣鍵漸姦枧堅缣撿鞯裥踺減鑑謇笕搛儉牮殱偂檢瞼艦濺毽趼殲倹楗洊鹣薦俴鑒剱劒挸検揀彅瑊澗賎監箋鲣浅墹踐湕繭艱鹼樫磵惤鞬劗戔戩緘鋻瞷劔縑瀸諓籛餞礛譾熞珔菺襉椷蕑櫼鰹藆襺螹瑐鵳糋釼鹸鬋虃鰎韉轞猏寋擶葌麉"};
//jiang
//const unsigned char PY_mb_jiang []={"将讲江奖降浆僵姜酱蒋疆匠强桨虹豇礓缰犟耩绛茳糨洚"};
const unsigned char PY_mb_jiang []={"将讲江奖降姜蒋酱浆將僵疆匠绛桨講犟獎缰丬蔣洚殭耩豇弶茳醬礓薑絳傋漿糨槳畺櫤奨虹强謽奬醤橿膙螿韁顜袶嵹杢翞"};
//jiao
//const unsigned char PY_mb_jiao  []={"叫脚交角教较缴觉焦胶娇绞校搅骄狡浇矫郊嚼蕉轿窖椒礁饺铰酵侥剿徼艽僬蛟敫峤跤姣皎茭鹪噍醮佼湫鲛挢"};
const unsigned char PY_mb_jiao  []={"叫交教较脚角胶焦娇觉浇搅缴嚼矫绞椒郊轿骄窖礁蕉姣蛟饺皎铰剿狡跤校滘較酵鲛腳侥鐎醮徼佼攪敎茭悎繳湫笅儌鹪挢噍峤敫膠憍嬌僬撹艽澆驕灚筊絞皦僥獥矯劋斠勦嘂簥挍轎爝珓膲憿摷鵁皭撟鉸鮫餃曒釂煍蟭樔譥嶕敿鷮穚嬓趭嘄纐鷦鵤鱎"};
//jie
//const unsigned char PY_mb_jie   []={"接节街借皆截解界结届姐揭戒介阶劫芥竭洁疥藉价楷秸桔杰捷诫睫家偈桀喈拮骱羯蚧嗟颉鲒婕碣讦孑疖诘"};
const unsigned char PY_mb_jie   []={"接借解节姐街界结杰皆届戒洁介截捷揭劫阶婕嗟藉湝竭尐孑喈桀诫芥睫颉羯诘卩桔結蚧讦傑節秸劼玠疖偈疥碣紒屆拮堺唶潔階崨家掲曁誡骱岕倢檞詰鲒訐滐褯耤楬岊狤癤妎袺楷蝍衱阂痎犗迼砎刧桝趌楶价頡昅嵥跲幯鉣蛶鞊踕畍蝔鍻鞂蠽巀榤鶛媎"};
//jin
//const unsigned char PY_mb_jin   []={"进近今仅紧金斤尽劲禁浸锦晋筋津谨巾襟烬靳廑瑾馑槿衿堇荩矜噤缙卺妗赆觐"};
const unsigned char PY_mb_jin   []={"进金近仅尽紧今劲斤锦禁晋津筋浸谨靳瑾巾噤襟矜進堇槿盡缙衿烬觐瑨緊僅妗荩勁錦晉殣煡謹枃馑卺賮溍寖赆菫廑祲僸儘笒紟侭巹搢嚍璶燼嬧璡蓳觔藎縉覲贐壗珒惍堻饉濜榗"};
//jing
//const unsigned char PY_mb_jing  []={"竟静井惊经镜京净敬精景警竞境径荆晶鲸粳颈兢茎睛劲痉靖肼獍阱腈弪刭憬婧胫菁儆旌迳靓泾"};
const unsigned char PY_mb_jing  []={"经静竟精景净京井惊镜敬晶警颈境竞靖径劲茎荆菁婧鲸睛泾旌璟殑經胫兢靜迳粳阱肼腈痉憬驚儆鏡淨刭靓弪競瀞経頸蟼獍亰丼徑凈倞荊弳莖暻鯨浄劤汫橸逕鶄旍鼱穽憼涇痙婛幜獷桱竸麠竫頚璥剄鵛鶁"};
//jiong
//const unsigned char PY_mb_jiong []={"窘炯扃迥"};
const unsigned char PY_mb_jiong []={"囧迥炯窘冂冏煚扃炅熲臦浻泂烱褧坰駉絅臩燛逈昋"};
//jiu
//const unsigned char PY_mb_jiu   []={"就九酒旧久揪救纠舅究韭厩臼玖灸咎疚赳鹫蹴僦柩桕鬏鸠阄啾"};
const unsigned char PY_mb_jiu   []={"就酒久旧九救揪玖究舅纠鸠灸咎臼鹫韭僦啾厩舊疚赳桕柩阄鳩鬏糾鷲殧勼韮糺萛廄氿慦杦揫柾廐廏鬮朻麔鯦"};
//ju
//const unsigned char PY_mb_ju    []={"句举巨局具距锯剧居聚拘菊矩沮拒惧鞠狙驹且据柜桔俱车咀疽踞炬倨醵裾屦犋苴窭飓锔椐苣琚掬榘龃趄莒雎遽橘踽榉鞫钜讵枸"};
const unsigned char PY_mb_ju    []={"据局距句居具巨剧聚举菊俱拒矩锯鞠惧橘桔驹拘炬狙钜掬據焗讵踞遽沮裾劇姖苴椐莒琚疽雎榘咀舉锔倨苣閰屦倶袓榉拠椈飓趄柜鞫洰龃窭秬崌枸醵踽且筥懼鉅粔詎挶俥鋸泃埧岠侷駒挙輂罝车踘虡擧椇匊懅鵙岨蚷犋娵屨湨壉駏犑櫸淗蒟鶪駶澽豦襷涺颶狊蜛痀梮欅耟躆陱焣鶋腒驧蘜郹鋦鵴"};
//juan
//const unsigned char PY_mb_juan  []={"卷圈倦鹃捐娟眷绢鄄锩蠲镌狷桊涓隽"};
const unsigned char PY_mb_juan  []={"卷捐娟倦绢涓鹃眷圈隽劵镌狷鄄蠲捲桊鐫絹锩罥雋羂鵑朘呟睊帣睠鋑淃臇菤裐絭錈"};
//jue
//const unsigned char PY_mb_jue   []={"决绝觉角爵掘诀撅倔抉攫嚼脚桷噱橛嗟觖劂爝矍镢獗珏崛蕨噘谲蹶孓厥"};
const unsigned char PY_mb_jue   []={"觉绝决角掘诀爵珏倔厥撅孓嚼噘蕨攫崛谲覺蹶絕抉玦傕橛噱矍決镢桷覚亅廴獗訣劂脚赽鷢絶憰钁觖駃蕝趉鐍芵爝譎玨鴃殌玃虳觼屩臄刔瘚貜镼鱖彏趹嶡鈌蟨戄"};
//jun
const unsigned char PY_mb_jun   []={"军君均菌俊峻骏龟郡珺隽竣骏钧浚畯焌晙莙郡筠麇皲捃"};

/*** K ***/
//ka
//const unsigned char PY_mb_ka    []={"卡喀咯咖胩咔佧"};
const unsigned char PY_mb_ka    []={"卡喀咖咔佧咯垰胩鉲擖裃"};
//kai
//const unsigned char PY_mb_kai   []={"开揩凯慨楷垲剀锎铠锴忾恺蒈"};
const unsigned char PY_mb_kai   []={"开凯楷恺開铠揩慨锴剀蒈凱忾垲暟锎欬鍇闓愷鎧剴颽烗輆勓塏鎎"};
//kan
//const unsigned char PY_mb_kan   []={"看砍堪刊嵌坎槛勘龛戡侃瞰莰阚"};
const unsigned char PY_mb_kan   []={"看砍坎侃刊堪勘阚槛龛瞰磡丬戡嵌竷矙顑莰崁墈檻衎栞轗"};
//kang
//const unsigned char PY_mb_kang  []={"抗炕扛糠康慷亢钪闶伉"};
const unsigned char PY_mb_kang  []={"抗康扛炕亢糠慷鈧伉钪闶摃囥犺鏮邟漮鱇嫝匟閌"};
//kao
//const unsigned char PY_mb_kao   []={"靠考烤拷栲犒尻铐"};
const unsigned char PY_mb_kao   []={"靠考烤拷尻铐栲犒洘攷銬薧稾"};
//ke
//const unsigned char PY_mb_ke    []={"咳可克棵科颗刻课客壳渴苛柯磕坷呵恪岢蝌缂蚵轲窠钶氪颏瞌锞稞珂髁疴嗑溘骒"};
const unsigned char PY_mb_ke    []={"可课克科客刻壳颗柯棵渴磕咳珂苛恪嗑坷髁轲颏缂蝌錒窠氪課峇敤岢瞌顆疴稞溘骒钶蚵鈳剋牁锞榼砢匼痾軻悈渇龕頦鉿濭堁搕磆毼騍薖萪嵙趷樖犐緙嵑翗嶱"};
//kei
const unsigned char PY_mb_kei    []={"尅剋勀"};
//ken
//const unsigned char PY_mb_ken   []={"肯啃恳垦裉"};
const unsigned char PY_mb_ken   []={"肯啃垦恳裉懇龈墾掯錹"};
//keng
//const unsigned char PY_mb_keng  []={"坑吭铿"};
const unsigned char PY_mb_keng  []={"坑吭铿脛鍞鏗阬誙牼銵硻"};
//kong
//const unsigned char PY_mb_kong  []={"空孔控恐倥崆箜"};
const unsigned char PY_mb_kong  []={"空孔控恐倥箜崆悾鞚硿錓"};
//kou
//const unsigned char PY_mb_kou   []={"口扣抠寇蔻芤眍筘叩"};
const unsigned char PY_mb_kou   []={"口扣抠寇叩蔻眍筘芤彄冦釦摳瞉滱怐鷇簆"};
//ku
//const unsigned char PY_mb_ku    []={"哭库苦枯裤窟酷刳骷喾堀绔"};
const unsigned char PY_mb_ku    []={"哭库苦酷裤枯窟堀刳绔庫骷褲喾楛矻袴朏搰瘔嚳跍捁桍胐狜"};
//kua
//const unsigned char PY_mb_kua   []={"跨垮挎夸胯侉"};
const unsigned char PY_mb_kua   []={"跨夸垮挎胯侉誇姱骻錁銙絓舿"};
//kuai
//const unsigned char PY_mb_kuai  []={"快块筷会侩哙蒯浍郐狯脍"};
const unsigned char PY_mb_kuai  []={"快块筷侩蒯会脍哙塊浍郐儈狯鱠澮鄶璯膾獪圦廥旝"};
//kuan
//const unsigned char PY_mb_kuan  []={"宽款髋"};
const unsigned char PY_mb_kuan  []={"款宽髋寛寬窾梡髖"};
//kuang
//const unsigned char PY_mb_kuang []={"矿筐狂框况旷匡眶诳邝纩夼诓圹贶哐"};
const unsigned char PY_mb_kuang []={"狂矿框况匡筐旷圹邝眶诳哐诓曠夼劻贶況纊誑纩狅鵟壙洭礦鄺丱鑛矌爌誆絖恇穬鉱懭貺軦軠絋昿筺砿"};
//kui
//const unsigned char PY_mb_kui   []={"亏愧奎窥溃葵魁馈盔傀岿匮愦揆睽跬聩篑喹逵暌蒉悝喟馗蝰隗夔"};
const unsigned char PY_mb_kui   []={"亏奎葵魁窥盔溃愧馈逵夔揆匮勹喹樻岿戣蒉虧潰蝰馗聩篑傀愦喟媿睽悝櫆匱窺隗暌煃刲跬饋蕢聧騤卼憒聵蹞頍嘳楏躨闚顝餽簣頄鐀嬇欳鍷蘬楑鄈藈頯"};
//kun
//const unsigned char PY_mb_kun   []={"捆困昆坤鲲锟髡琨醌阃悃"};
const unsigned char PY_mb_kun   []={"困坤昆捆琨鲲锟醌褌閫焜崑睏悃髡阃晜綑壼梱鰥鶤臗稛騉菎鯤豤裍錕涃猑硱"};
//kuo
//const unsigned char PY_mb_kuo   []={"阔扩括廓适蛞栝"};
const unsigned char PY_mb_kuo   []={"扩阔括廓闊擴蛞萿拡濶适懖筈鞹葀霩韕鬠"};


/*** L ***/
//la
//const unsigned char PY_mb_la    []={"拉啦辣蜡腊喇垃蓝落瘌邋砬剌旯"};
const unsigned char PY_mb_la    []={"啦拉喇辣蜡腊剌落垃邋砬菈瘌旯蠟臘溂鬎鞡臈楋柆翋藞蝋鑞揧"};
//lai
//const unsigned char PY_mb_lai   []={"来赖莱崃徕涞濑赉睐铼癞籁"};
const unsigned char PY_mb_lai   []={"来赖莱來濑睐癞徕赉籁涞賴唻瀬铼瀨萊崃棶騋癩頼崍徠籟錸淶睞娕藾箂誺庲鯠賚鶆"};
//lan
//const unsigned char PY_mb_lan   []={"蓝兰烂拦篮懒栏揽缆滥阑谰婪澜览榄岚褴镧斓罱漤"};
const unsigned char PY_mb_lan   []={"蓝烂兰懒栏岚拦揽篮滥览澜阑缆榄谰藍蘭斓镧爛繿懶嵐婪攬褴諫灆籃漤欄儖覧濫攔壈瀾罱覽嬾欖灠闌籣纜暕躝嚂灡襴欗譋爁襤浨斕擥醂糷燗讕爦鑭"};
//lang
//const unsigned char PY_mb_lang  []={"浪狼廊郎朗榔琅稂螂莨啷锒阆蒗"};
const unsigned char PY_mb_lang  []={"狼浪郎朗廊琅啷榔郞螂鎯阆莨埌崀閬锒烺塱蒗稂瑯誏羮硠鋃桹筤欴朖"};
//lao
//const unsigned char PY_mb_lao   []={"老捞牢劳烙涝落姥酪络佬潦耢铹醪铑唠栳崂痨"};
const unsigned char PY_mb_lao   []={"老捞劳牢佬烙涝唠姥痨酪铑落醪嫪崂勞咾鐒栳撈耢簩僗潦荖労蛯络铹恅銠橑澇癆嘮鮱浶軂橯狫顟嶗"};
//le
//const unsigned char PY_mb_le    []={"了乐勒肋仂叻泐鳓"};
const unsigned char PY_mb_le    []={"了乐勒叻仂肋樂楽泐牞鳓鰳竻氻砳阞"};
//lei
//const unsigned char PY_mb_lei   []={"类累泪雷垒勒擂蕾肋镭儡磊缧诔耒酹羸嫘檑嘞"};
const unsigned char PY_mb_lei   []={"类累雷泪磊蕾嘞垒勒镭肋擂類羸淚耒酹銇诔涙纍鑸缧儡罍嫘壘畾藟檑郲灅磥礌纇蔂絫縲塁礧鐳誄樏癗蘱蘲瓃讄虆轠鸓櫐礨蠝瘣壨禷頛"};
//leng
//const unsigned char PY_mb_leng  []={"冷棱楞塄愣"};
const unsigned char PY_mb_leng  []={"冷楞愣棱塄稜崚堎睖薐踜"};
//li
//const unsigned char PY_mb_li    []={"里离力立李例哩理利梨厘礼历丽吏犁黎篱狸漓鲤莉荔栗厉励砾傈俐痢粒沥隶璃俪俚郦坜苈莅蓠鹂笠鳢缡跞蜊锂悝澧粝枥蠡鬲呖砺嫠篥疠疬猁藜溧鲡戾栎唳醴轹詈骊罹逦喱雳黧蛎娌"};
const unsigned char PY_mb_li    []={"里李离力理立利丽例黎礼哩粒莉梨历厉沥栗厘吏励犁俐裡荔裏笠锂骊隶狸傈璃鲤篱俪喱郦痢戾砺砾漓藜澧離莅琍俚溧禮罹蠡刂麗呖唳鹂醴曞唎栎枥蛎娌詈疬逦蓠鬲褵缡坜欐豊苈雳鳢疠悧暦猁勵歷黧厤歴粝儷隸釐浬厲嫠悷曆蜊劙籬涖篥轹瀝慄磿儮瓅苙蒞瑮剺栵鯉瓥嚦栃鲡醨濿孋扐犂謧灕礪樆鋰檪砅蘺蠣縭驪朸粴轢櫪癘讈跞癧岦廲蠫搮茘盭娳赲屴菞刕隷秝蔾鱧鴗邐鑗蜧壢欚靋礰峛蚸漦禲鬁犡轣藶蠇鱺糲筣鵹糎靂孷麜鯏竰鱱鷅塛鸝鯬蒚"};
//lia
//const unsigned char PY_mb_lia	[]={"俩"};
const unsigned char PY_mb_lia	[]={"俩倆"};
//lian
//const unsigned char PY_mb_lian  []={"连联练莲恋脸炼链敛怜廉帘镰涟蠊琏殓蔹鲢奁潋臁裢濂裣楝"};
const unsigned char PY_mb_lian  []={"连练脸联恋莲链炼帘怜廉敛連濂涟琏鏈殮镰殓臉戀潋鍊楝鲢蓮奁練聯蠊堜煉斂湅臁鐮簾憐槤裢鎌裣縺蔹僆漣聨璉歛薕錬嗹謰翴鬑磏鰊奩浰襝匳瀲覝鰱摙褳澰鄻蹥萰羷嬚"};
//liang
//const unsigned char PY_mb_liang []={"两亮辆凉粮梁量良晾谅俩粱墚靓踉椋魉莨"};
const unsigned char PY_mb_liang []={"两量亮梁良凉辆靓粮俩晾谅粱兩椋靚樑魉涼悢踉両諒糧喨俍倞輛墚湸裲駺緉輌簗鍄綡啢蜋魎輬"};
//liao
//const unsigned char PY_mb_liao  []={"了料撩聊撂疗廖燎辽僚寥镣潦钌蓼尥寮缭獠鹩嘹"};
const unsigned char PY_mb_liao  []={"了聊料廖辽疗撩寮撂僚燎暸寥蓼嘹缭潦獠憭镣鐐钌瞭尥鹩繚敹療遼釕樛嫽憀摎窷尞鄝窌膫蟧鷯膋蟟嵺豂炓轑簝镽飉屪嶚飂"};
//lie
//const unsigned char PY_mb_lie   []={"列裂猎劣烈咧埒捩鬣趔躐冽洌"};
const unsigned char PY_mb_lie   []={"列咧烈裂猎劣冽洌鬣捩趔埒睙姴躐獵儠鴷埓颲櫑迾綟毟蛚茢猟擸鮤犣"};
//lin
//const unsigned char PY_mb_lin   []={"林临淋邻磷鳞赁吝拎琳霖凛遴嶙蔺粼麟躏辚廪懔瞵檩膦啉"};
const unsigned char PY_mb_lin   []={"林临琳淋霖邻磷麟拎鳞凛蔺吝啉璘膦廪赁粼臨閵檩遴懔嶙辚疄箖鄰驎亃躏燐瞵潾鱗凜繗廩菻僯賃躪悋鏻惏躙懍轔隣藺橉顲翷蹸壣暽檁碄焛甐"};
//ling
//const unsigned char PY_mb_ling  []={"另令领零铃玲灵岭龄凌陵菱伶羚棱翎蛉苓绫瓴酃呤泠棂柃鲮聆囹"};
const unsigned char PY_mb_ling  []={"另令领灵零凌玲岭陵铃龄菱〇翎伶绫泠苓羚聆呤靈棱棂領瓴鈴蛉夌綾囹酃舲柃嶺齡淩霊鲮澪倰崚彾炩岺蕶霝笭姈軨齢皊琌詅爧醽鴒鯪欞蔆錂輘櫺裬砱婈昤蘦坽駖朎狑"};
//liu
//const unsigned char PY_mb_liu   []={"六流留刘柳溜硫瘤榴琉馏碌陆绺锍鎏镏浏骝旒鹨熘遛"};
const unsigned char PY_mb_liu   []={"刘留六流柳溜瘤硫陆遛琉榴浏碌劉绺馏鎏熘骝旒蓅镏瑠蹓锍媹珋鹨嵧瀏鏐栁罶霤澑廇鰡懰餾騮鶹鎦畄嚠裗藰塯鷚綹飀璢蟉磟嬼飂雡麍"};
//long
//const unsigned char PY_mb_long  []={"龙拢笼聋隆垄弄咙窿陇垅胧珑茏泷栊癃砻"};
const unsigned char PY_mb_long  []={"龙隆弄笼拢聋陇垄珑龍胧泷垅窿栊槞咙茏竜砻曨籠儱癃攏蠪瀧聾壟鑨哢巃瓏隴嚨櫳襱矓篢蠬鸗朧躘礱谾徿梇蘢豅靇鏧霳嶐篭"};
//lo
const unsigned char PY_mb_lo   []={"咯"};
//lou
//const unsigned char PY_mb_lou   []={"楼搂漏陋露娄篓偻蝼镂蒌耧髅喽瘘嵝"};
const unsigned char PY_mb_lou   []={"楼喽漏露搂娄瘘陋篓镂樓蒌耧蝼嘍婁偻摟髅嵝廔僂瘺鏤瞜塿耬嶁螻艛窶熡鞻髏"};
//lu
//const unsigned char PY_mb_lu    []={"路露录鹿陆炉卢鲁卤芦颅庐碌掳绿虏赂戮潞禄麓六鲈栌渌蓼逯泸轳氇簏橹辂垆胪噜"};
const unsigned char PY_mb_lu    []={"路陆录鲁露卢炉鹿禄璐芦卤麓庐鹭噜颅虏潞掳戮撸泸橹碌漉绿赂渌逯鲈錄鏀鸬陸嚧盧垆鐪魯甪氇舻辂簬祿爐胪辘蕗六栌嚕镥廬稑鈩擄簏虜枦轳蘆櫓彔菉騄籙僇濾摝琭醁盝鹵穋鷺滷睩擼廘賂顱臚淥挔鱸坴纑翏觻鑪鏕瀘氌磠艫蔍艪璷踛轆錴熝壚轤硉螰漊蓾勠淕罏玈籚塶蹗舮蠦鸕魲艣鯥膟鵱"};
//luan
//const unsigned char PY_mb_luan  []={"乱卵滦峦孪挛栾銮脔娈鸾"};
const unsigned char PY_mb_luan  []={"乱卵栾鸾峦銮亂滦挛孪娈欒鸞鑾脔攣巒灤羉臠孌孿圞奱曫癵"};
//lue
//const unsigned char PY_mb_lue   []={"略掠锊"};
const unsigned char PY_mb_lue   []={"略掠锊畧鋝"};
//lun
//const unsigned char PY_mb_lun   []={"论轮抡伦沦仑纶囵"};
const unsigned char PY_mb_lun   []={"论轮伦纶仑抡沦論綸倫輪崘囵崙惀侖淪稐棆掄圇睔錀蜦菕溣碖踚陯"};
//luo
//const unsigned char PY_mb_luo   []={"落罗锣裸骡烙箩螺萝洛骆逻络咯荦漯蠃雒倮硌椤捋脶瘰摞泺珞镙猡"};
const unsigned char PY_mb_luo   []={"罗落洛咯裸骆络萝螺囉摞锣珞箩骡雒椤羅逻倮烙鏍欏镙蠃猡捋泺瘰荦漯蘿儸鑼猓欙脶絡茖駱纙峈籮犖皪礫儽邏攭臝詻鸁覶跞鮥蓏鱳腡騾攎剆玀鵅硌"};
//lv
//const unsigned char PY_mb_lv    []={"绿率铝驴旅屡滤吕律氯缕侣虑履偻膂榈闾捋褛稆"};
const unsigned char PY_mb_lv    []={"率绿吕铝驴旅律屡氯履滤虑侣缕捋闾綠褛呂垏寠榈屢慮繂閭驢絽偻縷膂嵂稆録侶鋁儢緑櫨梠褸葎勴簍謱藘膢慺櫚哷蔞瘻郘祣鑢鷜穭"};


/*** M ***/
//m
const unsigned char PY_mb_m    []={"呒嘸呣"};
//ma
//const unsigned char PY_mb_ma    []={"吗妈马嘛麻骂抹码玛蚂摩唛蟆犸嬷杩"};
const unsigned char PY_mb_ma    []={"吗嘛马妈骂码麻玛嗎抹馬么媽麽罵杩蟆蚂碼瑪孖鎷犸傌嘜嬤痲犘摩禡螞痳鬕蟇鷌溤榪礣"};
//mai
//const unsigned char PY_mb_mai   []={"买卖迈埋麦脉劢霾荬"};
const unsigned char PY_mb_mai   []={"买卖麦埋迈脉劢唛霾買賣麥売脈荬邁勱霢嘪鷶"};
//man
//const unsigned char PY_mb_man   []={"满慢瞒漫蛮蔓曼馒埋谩幔鳗墁螨镘颟鞔缦熳"};
const unsigned char PY_mb_man   []={"慢满蛮曼漫瞒蔓螨熳幔鳗滿蠻谩馒嫚缦鬘颟顢鏝姏墁镘埋僈縵瞞睌謾慲獌蘰矕鰻樠蟃屘饅鬗鞔"};
//mang
//const unsigned char PY_mb_mang  []={"忙芒盲莽茫氓硭邙蟒漭"};
const unsigned char PY_mb_mang  []={"忙盲芒莽茫蟒氓厖漭笀邙硭鋩牻硥汒駹蠎哤杗龒蘉娏痝蛖壾"};
//mao
//const unsigned char PY_mb_mao   []={"毛冒帽猫矛卯貌茂贸铆锚茅耄茆瑁蝥髦懋昴牦瞀峁袤蟊旄泖"};
const unsigned char PY_mb_mao   []={"毛猫冇冒茂帽茅矛貌贸卯锚懋铆貓昴旄茆瑁峁髦耄牦袤泖蟊瞀氂秏蝥楙芼鄚眊鄮貿媢犛冐皃錨酕蓩鉚軞枆毷渵蝐鶜"};
//me
//const unsigned char PY_mb_me    []={"么麽"};
const unsigned char PY_mb_me    []={"么麽麼嚜"};
//mei
//const unsigned char PY_mb_mei   []={"没每煤镁美酶妹枚霉玫眉梅寐昧媒糜媚谜沫嵋猸袂湄浼鹛莓魅镅楣"};
const unsigned char PY_mb_mei   []={"没每美梅妹煤眉酶枚媚霉媒玫镁沒魅昧寐湄莓袂楣浼沬嵋脢媺毎鎂媄渼糜鹛煝郿眛镅苺楳猸禖黴瑂痗徾挴燘蘪躾栂腜蝞塺堳攗"};
//men
//const unsigned char PY_mb_men   []={"门们闷扪焖懑钔"};
const unsigned char PY_mb_men   []={"们门闷焖們門扪満悗悶懑亹鞔暪钔菛鍆燜殙捫璊懣椚虋"};
//meng
//const unsigned char PY_mb_meng  []={"猛梦蒙锰孟盟檬萌氓礞蜢勐懵甍蠓虻朦艋艨瞢"};
const unsigned char PY_mb_meng  []={"梦猛蒙孟盟萌锰懵檬夢朦勐濛虻蠓甍瞢礞艋蜢曚氓儚鼆矇艨懞幪萠鋂甿鄳罞庬錳饛蕄鄸氋鸏霥瓾"};
//mi
//const unsigned char PY_mb_mi    []={"米密迷眯蜜谜觅秘弥幂靡糜泌醚蘼縻咪汨麋祢猕弭谧芈脒宓敉嘧糸"};
const unsigned char PY_mb_mi    []={"米迷密咪蜜秘谜弥觅眯靡糜醚尓糸祢幂宓泌蘼谧弭芈汨嘧麋脒汩櫁冪尒謎彌縻覓敉瞇猕祕幎羋瀰謐穈冞蓂灖渳爢蔤蔝鼏覛榓麛幦漞峚銤滵醾樒羃葞獼蝆熐塓麊鸍"};
//mian
//const unsigned char PY_mb_mian  []={"面棉免绵眠缅勉冕娩腼湎眄沔黾渑"};
const unsigned char PY_mb_mian  []={"面免棉眠绵缅勉冕眄婂沔腼麵娩俛緬綿湎丏喕偭媔鮸渑櫋麺矊愐緜矏蝒靦葂麪"};
//miao
//const unsigned char PY_mb_miao  []={"秒苗庙妙描瞄藐渺眇缪缈邈淼喵杪鹋"};
const unsigned char PY_mb_miao  []={"秒苗妙庙喵瞄描淼渺缪邈藐缈鹋眇杪廟嫹緲訬媌竗篎鱙"};
//mie
//const unsigned char PY_mb_mie   []={"灭蔑咩篾蠛乜"};
const unsigned char PY_mb_mie   []={"咩灭乜蔑滅篾搣蠛懱衊幭薎鱴"};
//min
//const unsigned char PY_mb_min   []={"民抿敏闽皿悯珉愍缗闵玟苠泯黾鳘岷"};
const unsigned char PY_mb_min   []={"民敏闵闽珉抿旻泯皿岷悯憫敃愍缗玟黾笢湣旼崏苠閔潣敯緡閩鳘僶暋黽罠鈱蠠錉琝怋刡痻簢"};
//ming
//const unsigned char PY_mb_ming  []={"名明命鸣铭螟盟冥瞑暝茗溟酩"};
const unsigned char PY_mb_ming  []={"名明命鸣铭冥茗溟螟暝瞑洺酩銘鳴姳掵詺眳蓂覭榠嫇鄍慏"};
//miu
//const unsigned char PY_mb_miu   []={"谬缪"};
const unsigned char PY_mb_miu   []={"缪谬繆謬"};
//mo
//const unsigned char PY_mb_mo    []={"摸磨抹末膜墨没莫默魔模摩摹漠陌脉沫万无冒寞秣蓦麽茉嘿嬷蘑瘼殁镆嫫谟貊貘馍耱"};
const unsigned char PY_mb_mo    []={"莫摸魔末膜磨模墨抹摩默没沫陌漠馍茉摹麽谟銆蘑殁寞脉蓦么嫫耱嬷鏌妺貘貊莈秣镆黙靺冒攠歿万謨麿絔眽粖瘼劘枺藦嗼驀纆袹糢暯狢瀎饃瞙蟔蛨塻絈爅髍嘿"};
//mou
//const unsigned char PY_mb_mou   []={"某谋牟眸蛑鍪侔缪哞"};
const unsigned char PY_mb_mou   []={"某谋眸牟哞侔謀鍪蛑缪洠鉾麰鴾"};
//mu
//const unsigned char PY_mb_mu    []={"木母亩幕目墓牧牟模穆暮牡拇募慕睦姆姥钼毪坶沐仫苜"};
const unsigned char PY_mb_mu    []={"木母目亩穆墓姆慕牧幕暮沐募钼睦牡牟拇模苜呒畝仫坶霂莯牳毪幙縸踇鉬茻獏毣凩萺鉧炑楘砪峔氁蚞鞪艒姥畆慔"};


/*** N ***/
//n
const unsigned char PY_mb_n    []={"嗯唔"};
//na
//const unsigned char PY_mb_na    []={"那拿哪纳钠娜呐南衲捺镎肭"};
const unsigned char PY_mb_na    []={"那拿哪纳娜呐钠捺衲納拏肭雫镎靹鈉笝秅魶貀軜豽"};
//nai
//const unsigned char PY_mb_nai   []={"乃耐奶奈氖哪萘艿柰鼐佴"};
const unsigned char PY_mb_nai   []={"乃奶耐奈萘氖鼐柰倷艿迺褦佴廼渿孻釢耏錼"};
//nan
//const unsigned char PY_mb_nan   []={"难南男赧囡蝻楠喃腩"};
const unsigned char PY_mb_nan   []={"男难南喃楠囡難腩囝柟赧蝻莮娚湳枏嫨諵暔妠戁婻遖萳抩揇"};
//nang
//const unsigned char PY_mb_nang  []={"囊馕曩囔攮"};
const unsigned char PY_mb_nang  []={"囊囔馕曩涳攮擃嚢齉灢鬞"};
//nao
//const unsigned char PY_mb_nao   []={"闹脑恼挠淖孬铙瑙垴呶蛲猱硇"};
const unsigned char PY_mb_nao   []={"闹脑挠恼孬瑙呶淖鬧铙腦脳垴猱悩婥嫐閙蛲硇惱臑夒撓蟯碙嶩碯鐃峱獿"};
//ne
//const unsigned char PY_mb_ne    []={"呢哪那呐讷"};
const unsigned char PY_mb_ne    []={"呢哪呐讷吶訥眲"};
//nen
const unsigned char PY_mb_nen   []={"嫩恁"};
//neng
const unsigned char PY_mb_neng   []={"能螚"};
//nei
const unsigned char PY_mb_nei   []={"内那內馁哪餒氝腇"};
//ni
//const unsigned char PY_mb_ni    []={"你泥拟腻逆呢溺倪尼匿妮霓铌昵坭祢猊伲怩鲵睨旎"};
const unsigned char PY_mb_ni    []={"你拟尼呢泥妳妮腻逆倪昵匿霓溺袮睨铌旎坭児鲵怩伲嬭猊膩擬灄齯貎隬暱迡嫟苨柅晲惄檷麑縌薿誽婗馜抳郳鯢狔蜺眤鈮堄跜秜觬臡屔濔籾淣"};
//nian
//const unsigned char PY_mb_nian  []={"年念捻撵拈碾蔫粘廿黏辇鲇鲶埝"};
const unsigned char PY_mb_nian  []={"年念粘廿捻黏碾蔫撵拈唸卄辇辗鲇鲶埝姩撚哖鮎蹍輦跈鯰攆簐秊躎"};
//niang
//const unsigned char PY_mb_niang []={"娘酿"};
const unsigned char PY_mb_niang []={"娘酿孃嬢釀醸"};
//niao
//const unsigned char PY_mb_niao  []={"鸟尿袅茑脲嬲"};
const unsigned char PY_mb_niao  []={"鸟尿嬲脲袅溺鳥茑褭蔦嫋裊嬝樢"};
//nie
//const unsigned char PY_mb_nie   []={"捏镍聂孽涅镊啮陧蘖嗫臬蹑颞乜"};
const unsigned char PY_mb_nie   []={"捏聂涅镍孽乜颞啮嗫蘖蹑镊臬鑷鎳摰聶摄敜陧讘苶囓嚙齧躡櫱巕臲嵲峊踂孼蠥糱隉踙踗篞鑈嶭闑顳"};
//nin
const unsigned char PY_mb_nin   []={"您恁"};
//ning
//const unsigned char PY_mb_ning  []={"拧凝宁柠狞泞佞甯咛聍"};
const unsigned char PY_mb_ning  []={"宁凝拧柠甯佞咛寧狞苧泞寗侫儜擰聍檸寍嚀聹嬣薴獰濘鑏鸋"};
//niu
//const unsigned char PY_mb_niu   []={"牛扭纽钮狃忸妞拗"};
const unsigned char PY_mb_niu   []={"牛妞扭纽钮拗忸狃鈕紐怓杻炄"};
//nou
const unsigned char PY_mb_nou  []={"耨譨鎒羺"};
//nong
//const unsigned char PY_mb_nong  []={"弄浓农脓哝侬"};
const unsigned char PY_mb_nong  []={"弄农侬浓脓哝濃農儂膿襛穠噥醲繷齈"};
//nu
//const unsigned char PY_mb_nu    []={"怒努奴孥胬驽弩"};
const unsigned char PY_mb_nu    []={"怒奴努弩驽胬孥帑砮笯駑"};
//nuan
//const unsigned char PY_mb_nuan  []={"暖"};
const unsigned char PY_mb_nuan  []={"暖奻渜餪"};
//nue
const unsigned char PY_mb_nue   []={"虐疟谑"};
//nuo
//const unsigned char PY_mb_nuo   []={"挪诺懦糯娜喏傩锘搦"};
const unsigned char PY_mb_nuo   []={"诺挪喏糯傩懦諾锘搦娜儺梛懧糑袲逽橠"};
//nv
//const unsigned char PY_mb_nv    []={"女钕恧衄"};
const unsigned char PY_mb_nv    []={"女钕籹衄釹恧朒衂聏"};


/*** O ***/
const unsigned char PY_mb_o     []={"哦噢喔嚄筽"};
const unsigned char PY_mb_ou    []={"偶欧藕呕鸥殴耦瓯沤鏂区歐怄讴藲櫙嘔毆蕅鷗鴎甌漚熰吘慪腢"};

/*** P ***/
//const unsigned char PY_mb_pa    []={"怕爬趴啪耙扒帕琶派筢杷葩"};
//const unsigned char PY_mb_pai   []={"派排拍牌迫徘湃哌俳蒎"};
//const unsigned char PY_mb_pan   []={"盘盼判攀畔潘叛磐番般胖襻蟠袢泮拚爿蹒"};
//const unsigned char PY_mb_pang  []={"旁胖耪庞乓膀磅滂彷逄螃"};
//const unsigned char PY_mb_pao   []={"跑抛炮泡刨袍咆狍匏庖疱脬"};
//const unsigned char PY_mb_pei   []={"陪配赔呸胚佩培沛裴旆锫帔醅霈辔"};
//const unsigned char PY_mb_pen   []={"喷盆湓"};
//const unsigned char PY_mb_peng  []={"碰捧棚砰蓬朋彭鹏烹硼膨抨澎篷怦堋蟛嘭"};
//const unsigned char PY_mb_pi    []={"批皮披匹劈辟坯屁脾僻疲痞霹琵毗啤譬砒否貔丕圮媲癖仳擗郫甓枇睥蜱鼙邳吡陂铍庀罴埤纰陴淠噼蚍裨"};
//const unsigned char PY_mb_pian  []={"片篇骗偏便扁翩缏犏骈胼蹁谝"};
//const unsigned char PY_mb_piao  []={"票飘漂瓢朴螵莩嫖瞟殍缥嘌骠剽"};
//const unsigned char PY_mb_pie   []={"瞥撇苤氕"};
//const unsigned char PY_mb_pin   []={"品贫聘拼频嫔榀姘牝颦"};
//const unsigned char PY_mb_ping  []={"平凭瓶评屏乒萍苹坪冯娉鲆枰俜"};
//const unsigned char PY_mb_po    []={"破坡颇婆泼迫泊魄朴繁粕笸皤钋陂鄱攴叵珀钷"};
//const unsigned char PY_mb_pou   []={"剖掊裒"};
//const unsigned char PY_mb_pu    []={"扑铺谱脯仆蒲葡朴菩曝莆瀑埔圃浦堡普暴镨噗匍溥濮氆蹼璞镤僕"};
const unsigned char PY_mb_pa    []={"怕爬帕趴啪扒耙琶葩掱杷帊筢钯袙鈀舥跁汃"};
const unsigned char PY_mb_pai   []={"拍派排牌哌湃徘俳迫蒎棑簰犤鎃箄"};
const unsigned char PY_mb_pan   []={"盘潘判盼攀畔叛磐番泮蟠胖爿丬盤袢牉媻槃幋襻籓拚蹒磻牓鞶跘溿頖褩詊眅沜蹣碆蒰縏坢瀊"};
const unsigned char PY_mb_pang  []={"胖旁庞滂逄乓彷嗙膀磅螃尨雱耪龐汸徬夆舽"};
const unsigned char PY_mb_pao   []={"跑泡抛炮袍刨疱庖咆拋脟狍匏褜炰嚗砲脬瓟萢麃垉鉋袌鞄皰奅蚫麭穮颮"};
const unsigned char PY_mb_pei   []={"陪配赔培佩裴呸沛胚珮辔霈醅旆姵帔伂賠衃锫俖浿轡蓜柸茷琣裵陫毰怌"};
const unsigned char PY_mb_pen   []={"喷盆噴湓衯呠翉歕葐翸"};
const unsigned char PY_mb_peng  []={"碰彭鹏捧朋棚蓬烹硼澎砰膨篷嘭芃椪踫堋怦抨鵬弸蟛傰剻磞摓絣輣錋熢漰韸匉痭鬅搒軯篣髼憉恲閛鑝掽駍樥皏"};
const unsigned char PY_mb_pi    []={"批皮披匹劈辟坯屁脾僻疲痞霹琵毗啤譬砒否貔丕圮媲癖仳擗郫甓枇睥蜱鼙邳吡陂铍庀罴埤纰陴淠噼蚍裨"};
const unsigned char PY_mb_pian  []={"片篇骗偏便扁翩缏犏骈胼蹁谝"};
const unsigned char PY_mb_piao  []={"票飘漂瓢朴螵莩嫖瞟殍缥嘌骠剽"};
const unsigned char PY_mb_pie   []={"撇瞥丿苤氕嫳潎暼覕"};
const unsigned char PY_mb_pin   []={"品拼频贫聘嫔颦姘拚牝嬪蘋榀頻貧嚬玭矉顰砏驞礗"};
const unsigned char PY_mb_ping  []={"平凭评瓶屏萍坪苹娉乒枰呯鲆憑評倗帡俜甁洴泙屛岼砯玶蓱頩軿鉼荓淜輧鮃缾蛢甹郱艵塀竮涄"};
const unsigned char PY_mb_po    []={"破坡颇婆泼迫魄巿泊珀攴粕皤叵鄱陂钋頗泺搫潑櫇朴笸昢钷哱桲尃髆謈岥醱濼繁溌翍蒪岶猼馞釙鏺烞"};
const unsigned char PY_mb_pou   []={"剖抔婄掊棓裒垺襃捊"};
const unsigned char PY_mb_pu    []={"普铺扑谱浦蒲朴噗瀑仆葡璞圃濮埔溥曝莆菩脯堡蹼撲氆镨匍譜纀鋪樸舖僕鐠暴酺墣潽镤襆蒱抪舗鵏鯆痡駇烳獛鏷瞨"};


/*** Q ***/
//const unsigned char PY_mb_qi    []={"起其七气期齐器妻骑汽棋奇欺漆启戚柒岂砌弃泣祁凄企乞契歧祈栖畦脐崎稽迄缉沏讫旗祺颀骐屺岐蹊萁蕲桤憩芪荠萋芑汔亟鳍俟槭嘁蛴綦亓欹琪麒琦蜞圻杞葺碛淇祗耆绮"};
//const unsigned char PY_mb_qia   []={"恰卡掐洽袷葜髂"};
//const unsigned char PY_mb_qian  []={"前钱千牵浅签欠铅嵌钎迁钳乾谴谦潜歉纤扦遣黔堑仟岍钤褰箝掮搴倩慊悭愆虔芡荨缱佥芊阡肷茜椠犍骞"};
//const unsigned char PY_mb_qiang []={"强枪墙抢腔呛羌蔷将蜣跄戗襁戕炝镪锖锵羟樯嫱"};
//const unsigned char PY_mb_qiao  []={"桥瞧敲巧翘锹壳鞘撬悄俏窍雀乔侨峭橇樵荞跷硗憔谯鞒愀缲诮劁"};
//const unsigned char PY_mb_qie   []={"切且怯窃茄砌郄趄惬锲妾箧慊伽挈"};
//const unsigned char PY_mb_qin   []={"亲琴侵勤擒寝秦芹沁禽钦吣覃矜衾芩溱廑嗪螓噙揿檎锓"};
//const unsigned char PY_mb_qing  []={"请轻清青情晴氢倾庆擎顷亲卿氰圊謦檠箐苘蜻黥罄鲭磬綮"};
//const unsigned char PY_mb_qiong []={"穷琼跫穹邛蛩茕銎筇"};
//const unsigned char PY_mb_qiu   []={"求球秋丘泅仇邱囚酋龟楸蚯裘糗蝤巯逑俅虬赇鳅犰湫鼽遒"};
//const unsigned char PY_mb_qu    []={"去取区娶渠曲趋趣屈驱蛆躯龋戌蠼蘧祛蕖磲劬诎鸲阒麴癯衢黢璩氍觑蛐朐瞿岖苣"};
//const unsigned char PY_mb_quan  []={"全权劝圈拳犬泉券颧痊醛铨筌绻诠辁畎鬈悛蜷荃"};
//const unsigned char PY_mb_que   []={"却缺确雀瘸鹊炔榷阙阕悫"};
//const unsigned char PY_mb_qun   []={"群裙逡麇"};
const unsigned char PY_mb_qi    []={"其起期气器七齐奇骑妻启旗企棋漆弃琪琦岂戚祁汽绮欺契泣祈砌祺淇柒崎栖乞岐麒脐歧鳍凄畦亓骐杞沏耆氣迄圻嘁讫颀憩欹芪綦齊葺掑萁忔祇蛴桤汔蕲碛旂碁蹊啟棄騎槭気荠蟿蜞屺綺萋豈跂埼芑湇攲済啓倛悽玘騏俟斉棲棨唭騹亟稽碕湆娸鬐噐淒憇敧訖迉僛懠鰭盵臍棊郪婍禨慽蚚璂觭諆竒芞缉愒蚑扢毄呇慼鏚鼜躩榿鄿鬿藄纃栔疧褄朞緕咠肵桼鬾磎肐魌蟣捿邔焏緀磧釮濝籏鯕岓隑頎玂魕踑軝鵸罊稘蠐甈霋鶀艩麡鶈"};
const unsigned char PY_mb_qia   []={"掐恰洽卡髂葜袷冾帢鞐鮚愘硈殎酠"};
const unsigned char PY_mb_qian  []={"钱前千签欠浅牵迁潜倩铅谦乾茜遣黔嵌纤钳仟歉骞芊虔芡錢谴钎堑阡悭褰佥灊钤搴扦簽墘箝荨遷牽嗛缱淺愆慊靬謙掮蒨仱潛箞鉛銭槧傔肷汧刋椠揵岍鉗僉拑犍奷籤輤挳騫鈐唊棈鰜檶媊縴譴葥櫏歁繾岒韆鑯軡揃朁欿塹濳篏婜蕁騝篟顅雃馯籖鐱竏粁黚鑓騚攓蜸鶼攐鰬榩"};
const unsigned char PY_mb_qiang []={"强抢枪墙腔呛羌蔷羟強炝锵樯嫱搶唴戗牆镪戕槍嶈跄蜣彊嗆墻襁繈锖牄鎗矼瑲墏繦嬙漒鏹鏘斨檣薔鶬戧謒羥蹌蹡艢羻椌錆熗蘠"};
const unsigned char PY_mb_qiao  []={"桥敲巧乔瞧翘俏窍侨撬鞘樵悄跷谯峭愀锹橇壳荞峤橋喬憔殼诮翹蟜墝噭礉墽劁硗僑鞒繑譙磽蹻燋潐竅趫蹺帩荍誚摮蕎鍬磝缲譑犞嫶嶠雀嘺鄡礄躈釥幧趬髜踍頝郻"};
const unsigned char PY_mb_qie   []={"且切妾窃茄怯緁鍥惬锲箧挈郄鐑趄竊洯厒倢椄朅慊篋癿疌踥蛣蛪愜藒鯜伽"};
const unsigned char PY_mb_qin   []={"亲秦琴勤钦沁侵寝禽覃芹擒嗪衾噙親芩溱揿笉撳寢寑瘽儭吣欽慬檎廑螓锓靲懃墐矜忴駸瀙唚蘄頜庈昑扲梫嶔澿櫬菳藽斳埁顉耹鋟蚙埐螼菣鈙鳹嶜坅雂肣"};
const unsigned char PY_mb_qing  []={"请清情轻青庆晴倾卿氢擎請氰顷苘磬箐漀輕罄氫慶黥傾亲謦檠凊勍圊靑蜻淸鲭綮頃鑋靘渹碃掅郬樈廎硜葝擏"};
const unsigned char PY_mb_qiong []={"穷琼穹邛茕蛩窮跫瓊銎筇煢藭璚舼蛬惸藑憌瞏橩笻桏赹"};
const unsigned char PY_mb_qiu   []={"球求秋邱丘裘囚仇糗泅酋虬遒楸逑鳅萩湫毬俅巯犰赇蚯銶龟蝤捄絿鼽璆崷坵殏莍逎訄賕緧鰍觩虯巰鰽煪朹蟗鞦觓趥媝紌恘梂唒釓蛷蠤穐蝵鰌釚鯄龝盚鶖鮂"};
const unsigned char PY_mb_qu    []={"去区取曲佢娶屈渠趋趣驱祛瞿蛆躯龋衢區觑黢麴迲阒闃蘧璩嶇袪佉诎脥灈呿氍蕖紶朐劬蛐趨彡鼩詘岖鸲麹敺驅蠼軀癯磲臞覷鮈跼苣駆浀麮鐻趜欋趍胠翵螶鼁籧郥躣匷抾刞蟝軥匤絇鴝髷蠷竘焌鶌鱋菃駈阹淭葋齲坥斪"};
const unsigned char PY_mb_quan  []={"全权劝圈拳犬泉券颧痊醛铨筌绻诠辁畎鬈悛蜷荃"};
const unsigned char PY_mb_que   []={"却缺确雀卻阙瘸鹊阕炔榷悫皵確埆闕闋礐愨塙殻鵲蒛毃硞碻礭躤蚗慤搉"};
const unsigned char PY_mb_qun   []={"群裙夋囷逡麇羣箘麕宭峮"};

/*** R ***/
//const unsigned char PY_mb_ran   []={"染燃然冉髯苒蚺"};
//const unsigned char PY_mb_rang  []={"让嚷瓤攘壤穰禳"};
//const unsigned char PY_mb_rao   []={"饶绕扰荛桡娆"};
//const unsigned char PY_mb_re    []={"热若惹喏"};
//const unsigned char PY_mb_ren   []={"人任忍认刃仁韧妊纫壬饪轫仞荏葚衽稔"};
//const unsigned char PY_mb_reng  []={"扔仍"};
//const unsigned char PY_mb_ri    []={"日"};
//const unsigned char PY_mb_rong  []={"容绒融溶熔荣戎蓉冗茸榕狨嵘肜蝾"};
//const unsigned char PY_mb_rou   []={"肉揉柔糅蹂鞣"};
//const unsigned char PY_mb_ru    []={"如入汝儒茹乳褥辱蠕孺蓐襦铷嚅缛濡薷颥溽洳"};
//const unsigned char PY_mb_ruan  []={"软阮朊"};
//const unsigned char PY_mb_rui   []={"瑞蕊锐睿芮蚋枘蕤"};
//const unsigned char PY_mb_run   []={"闰润"};
//const unsigned char PY_mb_ruo   []={"若弱偌箬"};
const unsigned char PY_mb_ran   []={"然染燃冉髯苒蚺姌熯呥袡髥珃"};
const unsigned char PY_mb_rang  []={"让讓嚷壤瓤攘穰禳譲瀼蘘勷壌懹獽穣躟爙鬤"};
const unsigned char PY_mb_rao   []={"绕饶扰桡娆繞荛饒擾遶橈嬈犪襓隢"};
const unsigned char PY_mb_re    []={"热惹熱喏"};
const unsigned char PY_mb_ren   []={"人任忍认仁刃韧壬刄仞紝妊稔亻荏纫轫衽認葚仭饪姙韌屻芢腍牣涊軔栠扨袵紉綛銋荵訒靭肕飪靱鵀"};
const unsigned char PY_mb_reng  []={"仍扔礽陾芿"};
const unsigned char PY_mb_ri    []={"日鈤馹"};
const unsigned char PY_mb_rong  []={"荣容融蓉溶绒榕熔戎茸嵘冗榮蝾肜瑢栄狨嫆傇鎔媶絨瀜毧褣軵嶸爃穃茙峵榵髶氄蠑羢烿駥"};
const unsigned char PY_mb_rou   []={"肉柔揉厹鍒糅蹂鞣粈禸葇宍煣媃輮蝚楺騥鰇髳鶔莥韖"};
const unsigned char PY_mb_ru    []={"如入乳汝儒茹辱濡孺褥蠕嚅襦嬬洳溽铷缛蓐侞嗕曘媷込薷銣筎帤袽擩挐鄏颥獳縟蕠醹鴽杁蒘鳰"};
const unsigned char PY_mb_ruan  []={"软阮朊軟堧檽耎緛壖瓀蝡燸礝"};
const unsigned char PY_mb_rui   []={"瑞锐睿蕊芮蕤緌叡蚋汭枘銳兊桵蕋橤蘂繠"};
const unsigned char PY_mb_run   []={"润闰潤閏閠橍"};
const unsigned char PY_mb_ruo   []={"若弱偌箬爇婼渃蒻鄀篛楉撋鶸鰯嵶"};


/*** S ***/
//const unsigned char PY_mb_sa    []={"撒洒萨挲仨卅飒脎"};
//const unsigned char PY_mb_sai   []={"塞腮鳃思赛噻"};
//const unsigned char PY_mb_san   []={"三散伞叁馓糁毵霰"};
//const unsigned char PY_mb_sang  []={"桑丧嗓颡磉搡"};
//const unsigned char PY_mb_sao   []={"扫嫂搔骚梢埽鳋臊缫瘙"};
//const unsigned char PY_mb_se    []={"色涩瑟塞啬铯穑"};
//const unsigned char PY_mb_sen   []={"森"};
//const unsigned char PY_mb_seng  []={"僧"};
//const unsigned char PY_mb_sha   []={"杀沙啥纱傻砂刹莎厦煞杉嗄唼鲨霎铩痧裟挲歃"};
//const unsigned char PY_mb_shai  []={"晒筛色酾"};
//const unsigned char PY_mb_shan  []={"山闪衫善扇杉删煽单珊掺赡栅苫掸膳陕汕擅缮嬗蟮芟禅跚鄯潸鳝姗剡骟疝膻讪钐舢埏"};
//const unsigned char PY_mb_shang []={"上伤尚商赏晌墒汤裳熵觞绱殇垧"};
//const unsigned char PY_mb_shao  []={"少烧捎哨勺梢稍邵韶绍芍召鞘苕劭潲艄蛸筲"};
//const unsigned char PY_mb_she   []={"社射蛇设舌摄舍折涉赊赦慑奢歙厍畲猞麝滠佘"};
//const unsigned char PY_mb_shen  []={"身伸深婶神甚渗肾审申沈绅呻参砷什娠慎葚糁莘诜谂矧椹渖蜃哂胂"};
//const unsigned char PY_mb_sheng []={"声省剩生升绳胜盛圣甥牲乘晟渑眚笙嵊"};
//const unsigned char PY_mb_shi   []={"是使十时事室市石师试史式识虱矢拾屎驶始似嘘示士世柿匙拭誓逝势什殖峙嗜噬失适仕侍释饰氏狮食恃蚀视实施湿诗尸豕莳埘铈舐鲥鲺贳轼蓍筮炻谥弑酾螫"};
//const unsigned char PY_mb_shou  []={"手受收首守瘦授兽售熟寿艏狩绶"};
//const unsigned char PY_mb_shu   []={"书树数熟输梳叔属束术述蜀黍鼠淑赎孰蔬疏戍竖墅庶薯漱恕枢暑殊抒曙署舒姝摅秫"};
//const unsigned char PY_mb_shua  []={"刷耍唰"};
//const unsigned char PY_mb_shuai []={"摔甩率帅衰蟀"};
//const unsigned char PY_mb_shuan []={"栓拴闩涮"};
//const unsigned char PY_mb_shuang[]={"双霜爽泷孀"};
//const unsigned char PY_mb_shui  []={"水谁睡税说"};
//const unsigned char PY_mb_shun  []={"顺吮瞬舜"};
//const unsigned char PY_mb_shuo  []={"说数硕烁朔搠妁槊蒴铄"};
//const unsigned char PY_mb_si    []={"四死丝撕似私嘶思寺司斯食伺厕肆饲嗣巳耜驷兕蛳厮汜锶泗笥咝鸶姒厶缌祀澌俟"};
//const unsigned char PY_mb_song  []={"送松耸宋颂诵怂讼竦菘淞悚嵩凇崧忪"};
//const unsigned char PY_mb_sou   []={"艘搜擞嗽嗾嗖飕叟薮锼馊瞍溲螋擞"};
//const unsigned char PY_mb_su    []={"素速诉塑宿俗苏肃粟酥缩溯僳愫簌觫稣夙嗉谡蔌涑"};
//const unsigned char PY_mb_suan  []={"酸算蒜狻"};
//const unsigned char PY_mb_sui   []={"岁随碎虽穗遂尿隋髓绥隧祟眭谇濉邃燧荽睢"};
//const unsigned char PY_mb_sun   []={"孙损笋榫荪飧狲隼"};
//const unsigned char PY_mb_suo   []={"所缩锁琐索梭蓑莎唆挲睃嗍唢桫嗦娑羧"};
const unsigned char PY_mb_sa    []={"撒萨洒仨卅飒薩灑摋颯脎靸挲馺攃隡"};
const unsigned char PY_mb_sai   []={"赛塞噻腮鳃賽僿毢揌顋"};
const unsigned char PY_mb_san   []={"三散伞叁彡氵傘糁繖馓毵帴鬖鏒毿糂糝閐"};
const unsigned char PY_mb_sang  []={"桑丧嗓搡喪颡褬磉顙鎟桒"};
const unsigned char PY_mb_sao   []={"扫骚嫂搔臊瘙掃騷缫埽鳋溞懆矂騒缲髞掻鐰鄵慅氉鱢"};
const unsigned char PY_mb_se    []={"色瑟涩塞啬拺铯濇穑澀銫渋粣鉍譅嗇璱澁穡濏栜廧鏼轖飋"};
const unsigned char PY_mb_sen   []={"森"};
const unsigned char PY_mb_seng  []={"僧"};
const unsigned char PY_mb_sha   []={"啥杀傻沙砂纱煞莎刹厦鲨痧殺唦猀霎裟濈紗剎铩箑鯊唼歃乷杉繌翣喢硰挲菨萐蔱翜樧嗄"};
const unsigned char PY_mb_shai  []={"晒筛曬色酾簁篩攦閷繺骰"};
const unsigned char PY_mb_shan  []={"山删闪善衫扇珊杉陕擅栅姗疝膳煽汕鳝缮跚单膻掸赡苫骟搧讪蟮睒閃刪嬗樿剡芟彡钐墠鄯鐥潸笘禅僤敾纔単繕掞晱舢墡煔姍羶埏柵剼陝攙贍猭痁穇赸襂譱訕鱔灗閊圸蔪謆杣騸髟覢饍狦"};
const unsigned char PY_mb_shang []={"上商伤尚赏殇裳熵觞墒傷晌绱賞仩垧尙殤汤塲慯觴謪蠰鬺螪蔏恦"};
const unsigned char PY_mb_shao  []={"少烧稍邵绍勺哨梢韶捎芍劭苕燒杓召蛸紹潲艄卲筲焼鞘弰旓髾玿輎蕱袑鮹"};
const unsigned char PY_mb_she   []={"设社射蛇舍摄舌涉佘奢赦赊慑畲折麝歙攝設厍闍滠厙捨涻揲蠂舎渉賒懾韘猞蔎檨輋挕慴騇"};
const unsigned char PY_mb_shen  []={"神深沈身甚申肾伸审什慎渗谂婶砷参珅绅燊哂屾莘呻蜃柛諗鰰審兟诜矧糁腎侁脤胂榊妽娠蔘幓椹嫀甡伔渖葚嬸淰綝棯侺槮滲堔訷紳瀋鯓愼抌詵葠氠讅瞫罧駪邥眒鰺覾峷曋祳鯵鋠籸"};
const unsigned char PY_mb_sheng []={"生省声升胜盛圣剩绳晟笙聖牲聲昇勝甥乘枡嵊眚泩偗剰陞繩湦賸渻憴鉎渑乗貹呏焺譝澠橳鼪曻鵿"};
const unsigned char PY_mb_shi   []={"是时事使市式试石十室师诗食史世实施视氏士湿失识饰始屎势适尸仕拾示時释似逝狮誓侍嗜驶蚀矢什拭虱噬匙恃柿礻弑丗饣谥詩師實湜轼峙飾舐試豕視適莳識铈筮蓍釋寔殖褷忕実勢屍埘奭濕辻諟溡弒栻崼塒蝕炻鲺駛螫獅鲥諡贳簭祏酾咶溼遰鍉蒔湤飭釈恀榯蝨鉽鰣澨笶忯鰤葹鈰觢鼭絁鼫邿襫貰枾遾釃鮖溮篒鎩軾鳲榁竍襹餝揓鶳嘘烒"};
const unsigned char PY_mb_shou  []={"收手受首瘦守售兽寿授熟収扌狩绶痩壽艏獸綬獣"};
const unsigned char PY_mb_shu   []={"书数树输属熟术舒鼠叔淑束疏署述竖俞蜀梳孰殊姝恕赎暑漱薯墅枢庶抒曙戍書數澍蔬氀塾黍樹丨倏纾屬菽沭輸沐殳術秫紓鼡疋豎腧摅毹潻綀樞朮贖糬疎鉥婌杸橾癙陎璹鄃竪襡軗鏣攄虪踈襩裋怷跾焂鮛鵨鶐"};
const unsigned char PY_mb_shua  []={"刷耍唰"};
const unsigned char PY_mb_shuai []={"帅甩摔衰率蟀帥"};
const unsigned char PY_mb_shuan []={"栓拴涮闩閂"};
const unsigned char PY_mb_shuang[]={"爽双霜雙孀泷滝塽孇樉驦漺礵慡艭灀縔鷞"};
const unsigned char PY_mb_shui  []={"水谁睡税说娷閖涚帨祱氺"};
const unsigned char PY_mb_shun  []={"顺舜瞬吮巛順楯蕣橓輴瞚鬊"};
const unsigned char PY_mb_shuo  []={"说說硕朔烁数铄槊搠妁蒴碩爍鎙"};
const unsigned char PY_mb_si    []={"死四思斯似司丝寺私撕肆巳嗣饲祀食泗厮厶嘶纟俟伺锶驷姒汜涘絲耜笥兕俬価偲咝澌缌鸶洍蛳虒儩颸廝飼柶竢駟泀鼶緦凘媤楒貄蟴蕬梩禠覗蕼鉰肂禗榹罳螄鷥"};
const unsigned char PY_mb_song  []={"送宋松颂诵耸嵩怂讼淞崧菘悚竦凇誦忪鎹鬆頌愯娀訟聳傱倯枩濍鍶漎硹慫駷棇憁蜙蘴嵷"};
const unsigned char PY_mb_sou   []={"搜艘叟馊嗖嗽薮廋擞溲螋飕瞍嗾捜蒐锼醙藪籔櫢擻餿欶鄋鎪獀颾颼騪"};
const unsigned char PY_mb_su    []={"素苏速俗诉宿酥塑粟肃溯夙蘇窣愫愬稣谡僳涑簌甦嗉蔌訴缩肅傃觫粛棴摵憟嫊遡餗樕蹜縤囌穌鋉驌玊洬膆謖泝蘓櫯藗鷫榡鱐"};
const unsigned char PY_mb_suan  []={"算酸蒜狻匴筭痠"};
const unsigned char PY_mb_sui   []={"岁随虽遂碎穗隋髓绥夊睢雖隧祟隨歲邃燧眭鐩挼穂濉鏸尿谇誶荽綏歳脺繐倠繀砕粋賥瀡埣髄襚縗娞穟璲浽禭陏荾滖繸哸檖旞毸鐆"};
const unsigned char PY_mb_sun   []={"孙损笋隼荪榫孫飧損狲筍蓀潠喰蕵鎨搎槂猻薞"};
const unsigned char PY_mb_suo   []={"所锁索缩嗦唆莎梭琐娑蓑羧鎖惢嗍傞挲桫縮睃葰唢鮻嗩摍瑣鎍靃簑溹趖魦鎈縒莏逤簔"};



/*** T ***/
//const unsigned char PY_mb_ta    []={"他她它踏塔塌拓獭挞蹋溻趿鳎沓榻漯遢铊闼嗒"};
//const unsigned char PY_mb_tai   []={"太抬台态胎苔泰酞汰炱肽跆呔鲐钛薹邰骀"};
//const unsigned char PY_mb_tan   []={"谈叹探滩弹碳摊潭贪坛痰毯坦炭瘫谭坍檀袒郯昙忐钽锬澹镡覃"};
//const unsigned char PY_mb_tang  []={"躺趟堂糖汤塘烫倘淌唐搪棠膛螳樘羰醣瑭镗傥饧溏耥帑铴螗"};
//const unsigned char PY_mb_tao   []={"套掏逃桃讨淘涛滔陶绦萄鼗啕洮韬饕叨焘"};
//const unsigned char PY_mb_te    []={"特忑慝铽忒"};
//const unsigned char PY_mb_teng  []={"疼腾藤誊滕"};
//const unsigned char PY_mb_ti    []={"提替体题踢蹄剃剔梯锑啼涕嚏惕屉醍鹈绨缇倜裼逖荑悌"};
//const unsigned char PY_mb_tian  []={"天田添填甜舔恬腆佃掭钿阗忝殄畋"};
//const unsigned char PY_mb_tiao  []={"条跳挑调迢眺龆笤祧蜩髫佻窕鲦苕粜"};
//const unsigned char PY_mb_tie   []={"铁贴帖萜餮"};
//const unsigned char PY_mb_ting  []={"听停挺厅亭艇庭廷烃汀莛葶婷梃蜓霆町铤"};
//const unsigned char PY_mb_tong  []={"同通痛铜桶筒捅统童彤桐瞳恫侗酮潼茼仝砼峒恸佟嗵垌"};
//const unsigned char PY_mb_tou   []={"头偷透投钭骰"};
//const unsigned char PY_mb_tu    []={"土图兔涂吐秃突徒凸途屠堍荼菟钍酴"};
//const unsigned char PY_mb_tuan  []={"团湍疃抟彖"};
//const unsigned char PY_mb_tui   []={"腿推退褪颓蜕煺忒"};
//const unsigned char PY_mb_tun   []={"吞屯褪臀囤氽饨暾豚"};
//const unsigned char PY_mb_tuo   []={"拖脱托妥驮拓驼椭唾鸵陀魄乇佗坨庹沱柝柁橐跎箨酡砣鼍"};
const unsigned char PY_mb_ta    []={"他她它塔踏塌祂沓牠榻蹋铊挞獭闼褟溻遢遝趿鳎撻拓漯咜撘荅逹搨鉈誻鞳禢嚃嗒濌闥獺鰨澾鞈毾涾榙鎉鞜闒錔"};
const unsigned char PY_mb_tai   []={"太台泰抬胎态钛苔肽汰邰骀酞薹呔跆臺態鲐檯儓炱鈦擡咍鈶颱斄溙嬯坮燤箈籉鮐"};
const unsigned char PY_mb_tan   []={"谈弹探谭贪坛叹摊坦碳痰潭滩炭檀瘫毯昙钽談澹坍覃郯袒湠彈傝貪弾攤倓嘆譚壇惔潬憳怹忐歎醓僋锬灘癱菼曇儃壜癉醰罈譠撣嘾埮餤嗿繵賧藫裧貚黮憛鉭舕婒顃榃暺罎"};
const unsigned char PY_mb_tang  []={"汤唐堂糖趟躺烫塘棠淌倘膛溏镗湯搪樘瑭欓傥耥螳铴蹚醣帑燙鎲羰爣螗儻钂蝪惝煻摥踼鏜簜愓榶赯鎕漟矘蘯漡攩逿橖戃蓎嵣閶磄闛鐋薚鶶曭闣鼞"};
const unsigned char PY_mb_tao   []={"套逃陶涛讨桃淘掏韬滔夲绦焘洮萄饕弢討祹啕濤鼗綯匋韜慆嫍檮梼迯幍搯槄燾絛騊醄縚叨鋾鞀駣蜪錭"};
const unsigned char PY_mb_te    []={"特忒忑慝铽貣蟘鋱"};
const unsigned char PY_mb_tei    []={"忒"};
const unsigned char PY_mb_teng  []={"疼腾藤滕誊痋騰螣縢籐謄籘熥邆霯"};
const unsigned char PY_mb_ti    []={"提体题踢替梯剃蹄啼剔缇涕锑鍗屉體惕悌倜題扌鹈俶醍嚏厗逖绨崹偍洟薙褆殢軆荑稊渧惿躰詆緹珶徥悐啑綈裼迖籊鶗騠鯷徲姼趧蝭揥楴戻嵜鶙屜鵜鷤瓋鬄磃謕蕛褅銻鷈"};
const unsigned char PY_mb_tian  []={"天田填添甜舔恬钿忝阗畑殄腆掭倎畋畠湉佃搷屇悿晪甛緂沺舑婖菾琠窴鈿酟闐碵淟賟餂捵鴫磌痶顚靦鷆睼鷏"};
const unsigned char PY_mb_tiao  []={"条跳调挑眺苕條調庣迢蜩佻窕粜龆儵朓祧笤髫鲦脁宨岧蓨誂糶鞗鯈覜蓚嬥咷絩趒鰷鎥朷芀窱祒齠"};
const unsigned char PY_mb_tie   []={"贴帖铁貼萜鐵餮鐡僣呫鋨怗鉆揲惵蛈鴩驖"};
const unsigned char PY_mb_ting  []={"听挺停厅庭亭婷廷汀艇霆烃聽梃葶蜓町閮铤脡廳莛侹珽綎聴渟庁筳諪廰珵聤娗艼桯榳烴濎楟頲鋌嵉烶蝏鼮圢"};
const unsigned char PY_mb_tong  []={"同通痛铜童桶统筒捅桐瞳彤佟酮砼潼仝恸侗嗵僮偅統炵峒茼垌銅曈粡樋烔囲浵橦哃痌湩朣詷慟爞絧犝衕膧氃鮦狪鉖蓪餇"};
const unsigned char PY_mb_tou   []={"头投偷透頭骰钭偸妵鍮蘣黈敨紏斢埱"};
const unsigned char PY_mb_tu    []={"图土吐涂兔突徒凸途屠秃荼圖菟钍鍎塗堍図兎酴湥瑹莵禿捈瘏鋀腯葖稌蒤唋汢跿廜捸嵞駼涋梌峹鵚筡嶀鵌鼵鷵釷鶟鵵鷋"};
const unsigned char PY_mb_tuan  []={"团湍鏄團疃抟彖団槫褖煓慱墥糰貒猯鷒鷻"};
const unsigned char PY_mb_tui   []={"退推腿褪忒颓蜕煺俀騩頹魋蓷穨隤僓弚頽藬橔蹪怢駾"};
const unsigned char PY_mb_tun   []={"吞屯臀囤豚氽暾饨呑褪焞魨庉坉畽軘蜳芚錪飩"};
const unsigned char PY_mb_tuo   []={"托拖脱拓陀妥坨驼驮砣沱唾佗庹乇铊脫橐椭箨鸵柁柝侂鼍魄跎酡蟺馱託鮀侻駝飥駄騨嘽嫷紽跅橢驒莌楕鼉沰鴕毤碢袉籜毻岮堶魠"};


/*** U ***/

/*** V ***/

/*** W ***/
//const unsigned char PY_mb_wa    []={"挖瓦蛙哇娃洼凹袜佤娲腽"};
//const unsigned char PY_mb_wai   []={"外歪崴"};
//const unsigned char PY_mb_wan   []={"完万晚碗玩弯挽湾丸腕宛婉烷顽豌惋娩皖蔓莞脘蜿绾芄琬纨剜畹菀"};
//const unsigned char PY_mb_wang  []={"望忘王往网亡枉旺汪妄芒罔惘辋魍"};
//const unsigned char PY_mb_wei   []={"为位未围喂胃微味尾伪威伟卫危违委魏唯维畏惟韦巍蔚谓尉潍纬慰桅萎苇渭遗葳帏艉鲔娓逶闱隈沩玮涠帷崴隗诿洧偎猥猬嵬軎韪炜煨圩薇痿"};
//const unsigned char PY_mb_wen   []={"问文闻稳温吻蚊纹瘟紊汶阌刎雯璺"};
//const unsigned char PY_mb_weng  []={"翁嗡瓮蓊蕹"};
//const unsigned char PY_mb_wo    []={"我握窝卧挝沃蜗涡斡倭幄龌肟莴喔渥硪"};
//const unsigned char PY_mb_wu    []={"无五屋物舞雾误捂污悟勿钨武戊务呜伍吴午吾侮乌毋恶诬芜巫晤梧坞妩蜈牾寤兀怃阢邬忤骛於鋈仵杌鹜婺迕痦芴焐唔庑鹉鼯浯圬"};
const unsigned char PY_mb_wa    []={"哇挖瓦娃蛙袜洼娲佤唲腽襪漥窪嗢窊窐媧韎韈瓲膃溛"};
const unsigned char PY_mb_wai   []={"外歪崴喎咼瀤"};
const unsigned char PY_mb_wan   []={"玩万完晚碗湾弯丸婉腕挽宛皖蔓绾顽烷夘椀剜琬萬菀卍脘纨蜿婠鍐惋莞畹晼笂潫彎灣晩掔綄豌芄関頑倇鄤汍翫琓槾刓澫綰紈薍捥輓綩岏杤鋄睕忨抏捖"};
const unsigned char PY_mb_wang  []={"网王往忘望汪旺亡妄枉罔網惘暀莣魍徃仼朢尪辋迋盳抂菵輞瀇"};
const unsigned char PY_mb_wei   []={"为未位喂味微伟维威魏胃卫委唯围韦薇伪尾谓惟為危炜玮蔚纬慰畏苇违巍尉渭萎爲煨偎囗帷圩葳鍏痿桅帏潍猥娓謂隗偉洧闈寪圍嵬隈維逶衛暐鍡闱溦偽沩韪浘噲猬濊韋煟艉涠遗緯幃瑋梶诿崴違餵菋韡僞崣儰廆葦鲔撱蒍煒洈鮠頠湋壝痏軎蓶溈瞶霨蝟碨諉衞趡濰蘶蔿癓鮇徫鮪餧霺峗媦藯喡縅磑芛詴犚椲鄬鏏蝛蜲渨峞蜼踓讆磈覹愄覣褽瀢癐犩巋潿躗椳鰄韙罻骫烓濻瓗揋斖隇饖腲葨"};
const unsigned char PY_mb_wen   []={"问文温稳闻吻纹雯蚊搵汶瘟問刎亠玟閺芠蕴炆聞紊溫閿紋穩璺脕阌闅妏顐瑥穏鰛呡絻馼忞呅塭魰殟豱抆歾桽鰮饂榲鳼鼤"};
const unsigned char PY_mb_weng  []={"翁瓮嗡滃蓊甕蕹聬奣螉齆暡罋鶲瞈塕"};
const unsigned char PY_mb_wo    []={"我喔窝握卧沃硪涡倭渥蜗肟斡偓幄臥挝龌堝窩濄莴仴涴瓁渦雘擭踠齷踒腛蝸萵猧焥撾婑濣捼瞃婐捰薶"};
const unsigned char PY_mb_wu    []={"无五唔吴物吾舞武屋误乌雾勿伍务午悟捂巫污呜毋戊邬無钨兀坞晤梧芜吳诬摀侮忤妩婺鹜蜈仵骛屼庑寤焐於怃洖圬誤堥浯痦烏鼯芴霧務憮鋈嗚阢洿倵蕪橆汙誣潕譕珷杌禑沕亡鹉俉迕靰嫵呉粅躌牾鄔塢啎廡旿墲剭嵨郚騖膴莁玝霿扤碔幠蘁窏鎢鷡杇瞴趶鯃岉雺恶甒逜鵡茣錻螐珸鵐鶩矹鋘釫鴮齀"};


/*** X ***/
//const unsigned char PY_mb_xi    []={"西洗细吸戏系喜席稀溪熄锡膝息袭惜习嘻夕悉矽熙希檄牺晰昔媳硒铣烯腊析隙栖汐犀蜥奚浠葸饩屣玺嬉禊兮翕穸禧僖淅蓰舾蹊醯欷皙蟋羲茜徙隰郗唏曦螅歙樨阋粞熹觋菥鼷裼舄"};
//const unsigned char PY_mb_xia   []={"下吓夏峡虾瞎霞狭匣侠辖厦暇唬狎遐瑕柙硖罅黠呷"};
//const unsigned char PY_mb_xian  []={"先线县现显掀闲献嫌陷险鲜弦衔馅限咸锨仙腺贤纤宪舷涎羡铣见苋藓岘痫莶籼娴蚬猃祆冼燹跣跹酰暹氙鹇筅霰"};
//const unsigned char PY_mb_xiang []={"想向象项响香乡相像箱巷享镶厢降翔祥橡详湘襄芗葙饷庠骧缃蟓鲞飨"};
//const unsigned char PY_mb_xiao  []={"小笑消削销萧效宵晓肖孝硝啸霄哮嚣校骁哓潇逍枭绡淆崤箫枵筱魈蛸"};
//const unsigned char PY_mb_xie   []={"写些鞋歇斜血谢卸挟屑蟹泻懈泄楔邪协械谐蝎携胁解契叶绁颉缬獬榭廨撷偕瀣渫亵榍邂薤躞燮勰"};
//const unsigned char PY_mb_xin   []={"新心欣信芯薪锌辛寻衅忻歆囟莘馨鑫昕镡"};
//const unsigned char PY_mb_xing  []={"性行型形星醒姓腥刑杏兴幸邢猩惺省硎悻荥陉擤荇"};
//const unsigned char PY_mb_xiong []={"胸雄凶兄熊汹匈芎"};
//const unsigned char PY_mb_xiu   []={"修锈绣休羞宿嗅袖秀朽臭咻岫馐庥溴鸺貅髹"};
//const unsigned char PY_mb_xu    []={"许须需虚嘘蓄续序叙畜絮婿戌徐旭绪吁酗恤墟糈勖栩浒蓿顼圩洫胥醑诩溆煦盱砉"};
//const unsigned char PY_mb_xuan  []={"选悬旋玄宣喧轩绚眩癣券儇炫谖萱揎泫渲漩璇楦暄煊碹铉镟痃"};
//const unsigned char PY_mb_xue   []={"学雪血靴穴削薛踅噱鳕泶"};
//const unsigned char PY_mb_xun   []={"寻讯熏训循殉旬巡迅驯汛逊勋荤询浚巽埙荀蕈薰峋徇獯恂洵浔曛醺鲟郇窨"};
const unsigned char PY_mb_xi    []={"系西洗戏喜係细吸希溪锡息兮熙席惜稀习夕袭曦昔悉膝析奚玺硒隙熄腊嘻禧烯汐栖犀嬉徙羲熹茜晰匸铣浠戲媳鎴矽細皙僖晞郗莃醯蜥窸觋莔淅唏檄翕牺舄豨橀繫熺屣悕蹊錫粞隰呬葸襲欷戯饩習郤螅蟋煕舾傒槢晳阋禊穸鑴樨菥歙瓕谿扱裼蓰卌瘜憙璽巂鸂鼷俙礂犠氥愾忥纚嶲盻碏潟枲惁諰蠵觿狶戱肸徯潝犧憘赩燨縰巇謵蓆虩睎郋覡嶍霫綌鬩銑鱚鈒覤蟢鄎咥摡焟恓暿凞酅徆譆渓鯑熂雟獡蹝餼貕釳鰓琋爔豯鰼恄飁黖驨蔇蒠廗蝷騱怬螇釸鵗葈闟榽衋蕮蜤騽桸緆欯屖鼰鼳橲漇"};
const unsigned char PY_mb_xia   []={"下夏吓瞎虾霞侠峡厦辖匣狭暇呷遐瑕狎昰芐梺蝦鍜嚇黠罅俠祫硖柙唬珨狹峽徦圷轄廈煆舺諕烚鰕謑笚蕸浹欱陜硤螛谺赮縖魻鶷舝鏬閜碬颬騢"};
const unsigned char PY_mb_xian  []={"先线县现咁闲仙显嫌贤限鲜险咸弦献陷宪馅掀腺衔娴见纤羡冼酰涎舷祆線現藓痫霰蚬籼岘賢苋癎铣縣閒暹锨憪顯険憲粯県鍌嫻跣跹閑獻蚿氙險鮮彡鹹鹇睍佡獮纖憸莶綫顕伭洗筅僩綅俔絃仚尠禒晛羨燹銜諴堿娊餡僊慳灦玁僴幰秈啣銛忺繊禰姺孅誸猃鏾溓蜆尟姭譀珗涀廯嫺筧韱豏峴壏蘚鶱盷赻譣撊陥獫鰔襳奾釤澖槏褼胘錎杴纎莧蘞埳攕顈羬癇蛝瀗躚馦屳鋧闞嘕毨櫶韅贒娹糮攇搟鷴烍鷳鼸"};
const unsigned char PY_mb_xiang []={"想向像象相香项乡箱响祥湘翔巷享详镶厢襄橡飨饷缃骧降芗庠鄉葙響項儴鲞瓨闀欀詳鑲蟓嚮姠珦郷瓖緗餉蠁饗啌廂萫襐纕驤曏鐌薌銗閧衖鄕膷饟忀鬨鱌晑鱶缿鯗"};
const unsigned char PY_mb_xiao  []={"小笑晓肖校萧消孝销效啸筱削箫霄潇宵硝枭逍骁嚣绡哮哓傚咲曉蕭庨魈淆篠笹虓簫崤嘯蛸暁箾銷猇潚皛枵効洨啋呺毊橚睄綃譊瀟憢嘐鴞囂梟骹詨灱翛漻謼斅嘵縿窙謏恷恔嗃穘膮莦萷謞獢轇歊涍痟燆藃髇蟂郩虈蠨熽髐驍踃薂揱痚鷍"};
const unsigned char PY_mb_xie   []={"写些谢鞋邪斜携协蟹歇泻血卸泄屑挟蝎胁谐械偕燮懈榭寫楔褉撷謝亵冩缬契解邂勰绁拹纈鮭躞獬廨薤瀉攜偰協瀣挾絜渫伳緳劦蠍脅澥愶滊瑎叶諧榍颉媟屧脇繲卨嶰揳僁灺褻屭觧躠擷襭熁觟垥恊蠏猲祄魼奊薢屓裌齛燲慀吤嗋蝢孈韰齥龤搚焎齘齂"};
const unsigned char PY_mb_xin   []={"新心信欣馨鑫辛芯锌薪昕歆忻莘忄衅鈊鐔伈囟炘潃噺杺镡枔鋅撢焮舋寻馫廞惞庍盺釁妡橝鬵愖礥阠"};
const unsigned char PY_mb_xing  []={"行性型星姓醒兴形幸刑邢杏腥荇惺猩煋硎擤興侀省饧倖洐悻陉荥垶婞狌陘涬哘瑆騂巠鈃鉶坙鋞娙裄莕箵餳觲蛵"};
const unsigned char PY_mb_xiong []={"熊兄雄胸凶匈夐芎汹兇敻洶詗赨恟忷哅"};
const unsigned char PY_mb_xiu   []={"修秀休绣袖羞嗅锈溴宿朽岫咻琇脩脙貅綉庥馐繡髹珛繍鸺茠鏽嚊臭滫烋銝櫹鏥鎀樇苬臹螑鵂饈鮴糔"};
const unsigned char PY_mb_xu    []={"需须徐许虚续旭序恤嘘叙蓄绪絮胥墟煦戌婿吁圩栩須許畜勖诩顼盱虛呴續緒酗繻歔浒蓿敘訏続垿溆敍銊醑侐噓姁勗湑糈鬚砉洫慉掝諝欻謳卹魆嬃冔詡昫旴聟珝緰暊壻諿喣鑐蕦幁魖楈縃稰漵頊訹殈蚼胊鄦欨蓲藚窢驉緖烅蝑晇揟沀烼瞁"};
const unsigned char PY_mb_xuan  []={"选玄轩宣悬旋炫萱璇喧眩癣绚铉暄煊漩瑄選渲泫烜煖儇楦谖軒鏇券玹禤懸县妶镟翾揎碹璿絢狟袨痃縼衒鉉蠉鞙諼鋗晅鐶癬嫙昡諠懁琄琁媗獧昍咺讂蜁揈蓒蝖贙轋愋駽鍹矎萲蔙愃"};
const unsigned char PY_mb_xue   []={"学血雪薛穴靴削彐學谑鳕踅噱趐泶壆吷泬謔轌鱈膤樰觷怴泧岤澩鷽梋嶨袕艝斈燢疶蒆瞲狘"};
const unsigned char PY_mb_xun   []={"寻讯训熏薰勋巡逊循迅旬询荀巽驯汛洵浚殉浔徇埙蕈鲟珣恂尋訓醺曛訊勳濬窨峋遜獯潯詢勲侚燂薫郇纁荨噀蟳葷噚眴勛馴鄩灥塤焄璕攳紃壎栒撏矄揗鱘燻襑咰愻蔒燖燅迿臐"};



/*** Y ***/
//const unsigned char PY_mb_ya    []={"呀压牙押芽鸭轧崖哑亚涯丫雅衙鸦讶蚜伢垭揠岈迓娅琊桠氩砑睚痖疋"};
//const unsigned char PY_mb_yan   []={"眼烟沿盐言演严咽淹炎掩厌宴岩研延堰验艳阉砚雁唁彦焰蜒衍谚燕颜阎焉奄厣菸魇琰滟焱筵赝兖恹檐湮谳偃胭晏闫俨郾鄢妍崦嫣罨酽餍鼹铅殷阽芫阏腌剡"};
//const unsigned char PY_mb_yang  []={"样养羊洋仰扬秧氧痒杨漾阳殃央鸯佯疡炀恙徉鞅泱蛘烊怏"};
//const unsigned char PY_mb_yao   []={"要摇药咬腰窑舀邀妖谣遥姚瑶耀尧钥侥夭爻吆崾徭幺珧杳轺曜肴鹞窈鳐疟陶约铫"};
//const unsigned char PY_mb_ye    []={"也夜业野叶爷页液掖腋冶噎耶咽曳椰邪谒邺晔烨揶铘靥"};
//const unsigned char PY_mb_yi    []={"一以已亿衣移依易医乙仪亦椅益倚姨翼译伊蛇遗食艾胰疑沂宜异彝壹蚁谊揖铱矣翌艺抑绎邑蛾屹尾役臆逸肄疫颐裔意毅忆义夷溢诣议怿痍镒癔怡驿旖熠酏翊欹峄圯殪嗌咦懿噫劓诒饴漪佚咿瘗猗眙羿弈苡荑仡佾贻钇缢迤刈悒黟翳弋奕蜴埸挹嶷薏呓轶镱"};
//const unsigned char PY_mb_yin   []={"因引印银音饮阴隐荫吟尹寅茵淫殷姻烟堙鄞喑夤胤龈吲圻狺垠霪蚓氤铟窨瘾洇茚"};
//const unsigned char PY_mb_ying  []={"应硬影营迎映蝇赢鹰英颖莹盈婴樱缨荧萤萦楹蓥瘿茔鹦媵莺璎郢嘤撄瑛滢潆嬴罂瀛膺荥颍"};
//const unsigned char PY_mb_yo    []={"哟育唷"};
//const unsigned char PY_mb_yong  []={"用涌永拥蛹勇雍咏泳佣踊痈庸臃恿俑壅墉喁慵邕镛甬鳙饔"};
//const unsigned char PY_mb_you   []={"有又由右油游幼优友铀忧尤犹诱悠邮酉佑幽釉攸卣侑莠莜莸呦囿宥柚猷牖铕疣蚰蚴蝣鱿黝鼬蝤繇"};
//const unsigned char PY_mb_yu    []={"与于欲鱼雨余遇语愈狱玉渔予誉育愚羽虞娱淤舆屿禹宇迂俞逾域芋郁谷吁盂喻峪御愉粥渝尉榆隅浴寓裕预豫驭蔚妪嵛雩馀阈窬鹆妤揄窳觎臾舁龉蓣煜钰谀纡於竽瑜禺聿欤俣伛圄鹬庾昱萸瘐谕鬻圉瘀熨饫毓燠腴狳菀蜮蝓"};
//const unsigned char PY_mb_yuan  []={"远员元院圆原愿园援猿怨冤源缘袁渊苑垣鸳辕圜鼋橼媛爰眢鸢掾芫沅瑗螈箢塬垸"};
//const unsigned char PY_mb_yue   []={"月越约跃阅乐岳悦曰说粤钥瀹钺刖龠栎樾"};
//const unsigned char PY_mb_yun   []={"云运晕允匀韵陨孕耘蕴酝郧员熨氲恽愠郓芸筠韫昀狁殒纭"};
const unsigned char PY_mb_ya    []={"呀丫压亚牙雅押鸭芽娅崖哑涯吖鸦圧劜伢衙轧蚜亞垭氩桠讶崕氬亜壓掗琊痖啞鴨玡睚砑厓鐚迓岈猰犽揠埡呾椏婭疋鴉蕥訝姶齾堐輅唖庌錏厊襾圔輵穵磍圠孲聐齖"};
const unsigned char PY_mb_yan   []={"眼烟演言严燕盐岩沿颜炎艳研延妍彦雁验焉阎咽宴闫掩厌焰晏嫣淹衍砚焱堰檐阉琰鄢円筵腌偃煙恹谚蜒滟閻奄魇俨閹湮谳酽郾胭阏晻顏芫厣兖鹽赝嬿挻饜顔嬮彥嚴験硏崦偐唁剡餍豔縯菸讠偣厭驗兗弇鼹艷罨龑菴綖巖艶塩褗喦硯铅洝嚥椼甗醃諺姸儼巘灩椻巌嵃顩揜醼埏簷厴讞厳沇鷃讌棪臙殷鴳娮鴈碞唌豜燄郔匽魘黶媕渰扊喭溎嵒嶮挩閆贗蝘釅淊裺漹鰋湺焔嵓壛牪鷰珚揅黭烻阽錟薟橪贋觾抁猒莚爓噞篶敥嬐酀齴驠虤礹壧曮隒鳱軅黫樮鶠黤楌騴萒孍躽齞狿鼴姲愝麙鳫鬳"};
const unsigned char PY_mb_yang  []={"样杨养阳羊洋扬痒氧央仰漾秧佯殃恙楊炀鞅樣泱鍚養陽様疡徉垟鸯炴揚烊蛘暘怏卬佒癢崵潒昜敭羕瑒姎鍈崸坱颺瀁瘍煬眻婸鴦珜鸉抰歍軮雵岟禓懩紻鐊胦攁鉠"};
const unsigned char PY_mb_yao   []={"要药咬腰姚妖摇耀邀尧遥瑶窑幺曜爻谣舀夭钥吆杳肴垚鹞侥藥傜鳐窈葽鑰喓徭繇偠愮猺崾搖媱薬珧遙窅瑤殽堯岆鴢疟謠祅么葯燿窯铫颻轺揺獟騕訞尭抭榣窔謡约嶢詏窰殀榚撽蕘靿曣顤摿噛銚嗂艞陶恌穾覞玅溔鰩軺驁鷂鷕苭眑袎烑筄宎餚柼"};
const unsigned char PY_mb_ye    []={"也页叶业夜野耶液爷冶椰烨晔噎曳掖咽谒葉腋邺靥揶業頁爺铘邪熀鄴暍倻堨枒謁緤埜瓛拽燁殗靨澲殕窫痷嶪鍱楪曄鎑擫鵺瞱蠮擛漜鐷饁抴歋擨"};
const unsigned char PY_mb_yi    []={"一以已亿易亦伊意依衣义宜仪乙益医矣逸异艺翼译移怡尾壹疑毅忆倚议姨咦遗夷奕溢抑颐椅裔役邑蚁懿驿谊弋轶贻翊漪疫彝胰弈沂乂佚揖辷弌屹羿咿诣潩翌熠翳義绎刈诒苡镱噫挹臆薏晹埸誒嶷迤槸猗圯铱艾悒缢儀譯怿饴镒旖呓燚億異钇峄瘗憶殪熼佾藝蟻醫荑遺鎰劓侇痍寲洩浥顗舣議褘黟癔肄拸墿蜴訳斁扆瀷衪俋駅誃儗詣钀偯貽彛泆勩誼頤崺繄枻眙佁簃錡齮彜廙埶裛昳鉯曵亄仡匜伿栘垼銥枍敼驛苅帟袘酏靉踦曀謚洢訑觺詒壱繹鶂嫕釔帠暆軼杙輗釶蓺歖扡劮禕宧畩懌鷁詑箷柂黓繶椸螘豷瑿殔寱毉浂紲珆絏飴迻靾嫛譺縊詍跇醷囈圛瘞熈峓謻掜貤狋熤鮨槷蘙戺澺沶蛜阣銕檹燱螔羛恞嗌瘱檍藙檥賹冝鏔搤欥蛦襼迆衵醳蛇桋膉郼匇轙唈羠嶧隿釴鐿艗萓霬鸃肊鯣鷾鷖贀焲芅跠艤饐耴媐鳦礒玴瓵耛黳陭顊嶬鷊稦熪齸燡"};
const unsigned char PY_mb_yin   []={"因引银音印阴饮尹隐淫殷吟瘾胤茵荫寅廴垠铟龈姻洇蚓銀湚陰飲喑霪夂鄞訢愔隱氤茚垔窨伒噾吲堙崟狺蔭禋湮犾絪夤闉廕癮裀諲垽瘖粌誾釿婬傿靷隠嚚殥堷憖飮檃凐鈏韾銦蟫圁梀荶讔駰蘟硍鷣螾檭霒苂酳朄歅霠酓蔩趛猌輑濦慇欭齗蒑烎齦"};
const unsigned char PY_mb_ying  []={"应赢英硬影营迎鹰盈颖映莹樱瑛婴蝇莺瀛夃滢缨萦嬴萤郢颍應荧楹璎膺茔瘿贏瑩蓥罂鹦潆穎櫻嘤營撄媵桜鷹渶柍嬰荥応灐俓蠅偀営瀯螢纓縈鶯煐暎瓔縄霙潁瀅嫈滎摬韺熒罃譻褮攖塋蛍覮謍籯濴瀴蝿頴鎣礯瀠蝧碤甖旲廮濚癭珱蠳攍鴬嚶韹蘡賏朠藀罌鱦巆甇梬鸚軈矨鶧"};
const unsigned char PY_mb_yo    []={"哟育唷"};
const unsigned char PY_mb_yong  []={"用永勇拥涌咏泳雍庸蛹佣甬俑慵镛壅痈邕噰墉灉詠踊傛鳙埇饔槦湧臃恿擁傭怺鏞喁澭筩鄘踴雝嗈癰揘廱禜愑嫞鷛醟慂滽悀塎嵱鰫郺鯒"};
const unsigned char PY_mb_you   []={"有又由油游友右优幽尤忧幼犹邮佑悠呦诱铀釉酉疣柚冘攸囿鼬尢侑祐猷宥瀀莜蚴铕遊優卣牖黝鱿猶莠斿憂莸蚰繇誘偤褎羑姷櫌纋懮狖銪峟蝤楢郵沋輶戭蝣泑槱苃迶訧麀庮銹怮蚘耰莤牰逌嚘怞浟秞鄾貁肬獶魷蕕蒏揂聈"};
const unsigned char PY_mb_yu    []={"与于鱼雨玉欲余语宇遇羽域愈预浴予於與御育逾渝喻郁裕瑜俞虞愚禹渔豫淤娱钰誉狱毓寓煜峪馀榆昱瘀屿隅亐舆妤芋吁谕愉聿驭魚彧盂熨鹆迂欤庾腴語阈禺瑀萸谀顒鬻楀嵛牏觎鹬竽湡餘纡圉妪閾俣雩箊鈺娛踰畬悆棜狳燠嵎慾繘閼龉圄堣矞堉揄遹饫噳窳臾瘐褕舁尉偊譽預漁悇唹貐鬱伃諭輿窬蜮燏俁獄蝓潏棫伛淯敔祤禦瘉崳僪嶼儥邘傴堬蔚癒貍馭魊薁萭寙丿喁紆谷娯歟杅兪蓣媮籲玗栯諛謣釪旟灪醧扜喩楡礇陓袬衧歈挧嫗爩隩隃齵戫鴥錥麌砡峿澦鱮汚藇穻鱊璵覦鸆鋙蝺鸒轝罭荢緎翑淢盓睮蕍鋊獝蕷欝稢蟈艅鷸鵒菸羭蘛澞鄅燰硢蓹噊蒮蘌歶貗齬鰅篽礜斞螸頨籅飫輍畭鯲鳿驈楰鴪鮽桙斔饇雓礖軉稶"};
const unsigned char PY_mb_yuan  []={"元远原源员园院愿圆袁缘苑怨媛冤渊援猿垣沅鸢鸳塬遠辕緣願瑗爰垸鼋圓園芫掾員圜橼棩淵嫄噮螈褑妴箢妧鎱眢薳惌縁媴湲悁蒝蚖杬蜎鵷灁轅渕駌寃謜鳶笎櫞輐夗薗鴛裷禐羱楥蝯騵葾獂鋺蝝肙嬽邍鼘黿榞蒬溒裫榬鶢邧"};
const unsigned char PY_mb_yue   []={"月越约曰岳悦粤跃阅乐说玥栎钺約瀹説樾哕閲龠刖閱悅钥箹粵籥躍鈅噦嶽趯軏鉞櫟爚蛻戉扚擽鸑臒鋭鑠蚎矱礿躒蘥禴焆鸙黦抈髺龥"};
const unsigned char PY_mb_yun   []={"晕云运韵芸孕允匀蕴昀雲耘筠陨熨韫愠恽殒酝枟運郧媼氲鋆賱韻暈郓伝纭氳贇煇勻沄蘊褞澐狁涒緷醞殞餫頵紜妘蕓慍隕繧薀篔玧韞轀縕熅奫鄆馻畇惲荺抎橒鄖熉馧蜵溳眃蝹鈗霣縜阭员齫韗"};


/*** Z ***/
const unsigned char PY_mb_za    []={"咋砸杂匝扎咂雜拶雑噈磼鉔雥襍"};
const unsigned char PY_mb_zai   []={"在再载仔宰哉栽灾崽甾載災賳渽"};
const unsigned char PY_mb_zan   []={"咱赞暂攒讚簪瓒昝錾趱偺贊暫瓚儹拶灒喒鐕糌讃賛鏨蹔囋臢噆寁禶饡酇"};
const unsigned char PY_mb_zang  []={"脏藏葬臧赃髒蔵驵奘牂臟臓賍贓弉"};
const unsigned char PY_mb_zao   []={"早造遭澡糟燥灶枣藻噪皂凿躁棗蚤唣慥皁璪璅繅傮煰梍譟竈簉繰蹧喿趮竃"};
const unsigned char PY_mb_ze    []={"则泽责择則仄啧沢澤咋赜帻崱昃幘厠擇責舴箦迮伬侧択蘀謮嘖賾樍稄庂捑襗滜鸅蠌"};
const unsigned char PY_mb_zei   []={"贼賊蠈鯽"};
const unsigned char PY_mb_zen   []={"怎谮譖譛囎"};
const unsigned char PY_mb_zeng  []={"曾增赠憎综甑増缯贈罾锃繒曽鄫磳縡璔矰橧"};
const unsigned char PY_mb_zha   []={"炸扎渣闸乍眨札诈榨咋轧喳吒铡揸鍘醡查柞咤搾栅楂砟紮痄蚱偞哳齄囃霅劄剳軋詐喥皻挓喋箚蜡奓煠閘潳揷扠鮓觰鰈溠譗苲挿鞢抯蚻蔖譇樝齟齇齰"};
const unsigned char PY_mb_zhai  []={"摘宅窄债斋寨翟砦齋哜择債斎瘵侧嚌駘檡捚骴厏粂簀鉙"};
const unsigned char PY_mb_zhan  []={"站占战展斩沾詹粘蘸盏湛栈瞻毡绽颤戦辗戰佔旃崭霑邅谵斬惉栴嵁嶄偡棧鳽搌盞氈詀綻跕輾欃鱣颭饘嫸桟薝襢譫蛅譧橏輚魙轏黵琖虥驏醆驙餰鸇"};
const unsigned char PY_mb_zhang []={"张涨长章帐掌账杖仗胀丈障彰璋樟漳張嶂瘴蟑幛獐鄣漲帳仉粻暲脹賬嫜傽扙鞝粀慞墇鱆騿弡瞕遧"};
const unsigned char PY_mb_zhao  []={"找照赵招着兆朝昭罩召诏肇钊沼棹爪趙櫂曌炤鍣垗箌詔啁笊皽妱旐釗鉊盄嘲瑵鮡釽狣菬駋鳭"};
const unsigned char PY_mb_zhe   []={"这着者折哲遮浙這喆辙褶蔗辄乇蛰锗蜇谪赭柘摺棏磔摂螫晢悊輒粍夂嫬鹧淛鍺謫轍蟄囁蟅輙謺鷓虴砓讋鮿聑馲矺"};
const unsigned char PY_mb_zhen  []={"真镇针阵震振珍枕帧朕贞甄臻诊祯桢疹禛斟侦圳榛赈箴鍖眞砧鎮轸陣鸩缜潧蓁胗樼浈針姫貞稹偵鎭瑧畛診椹禎獉瑱紾楨袗侲沴縝謓槇挋眕軫溱紖誫枮眹碪塡桭鍼賑絼揕鴆敶籈鬒葴珎轃寊遉栚鱵澵榐薽辴竧鈂裖駗抮靕朾縥蜄黰"};
const unsigned char PY_mb_zheng []={"正整证郑挣争症征政蒸睁铮筝怔峥凧拯诤鄭湞證爭氶钲証狰晸綪烝倀崝癥掙姃崢幀睜箏丁鮏媜埥鉦諍鯖炡撜錚篜眐糽脀浧聇猙埩塣掟"};
const unsigned char PY_mb_zhi   []={"之只至值指知直制纸治志支致智质置止枝汁职芝执植脂织痣滞芷掷酯址旨肢趾痔稚隻炙吱侄徵挚秩栀殖雉祗郅峙帋跖窒祉戠枳帜彘潪庢豸媞陟紙咫桎栉卮帙蛭贽挃忮摭誌執質織製厔亊胝骘職鉄値柣蜘鸷庤轵絷觯剬畤薙凪黹衹沚乿摯膣姪滯埴寘夛踯徴擲樀阯禔迣踬扺傂瘈紩偫謢稙臸軽滍歭氏觝擿椥鋕秷秖臷廌蚔軹緻轾貭礩幟栺恉銍櫍櫛淽鳷酈慹犆摨泜穉袟跱捗蹛瓡觶晊躓胵楖贄疻縶騺巵嚔鴙鑕识洷秪梔汥胑阤擳杝猘懥樲墆搘鷙螲懫隲狾樴扻瓆騭蘵鶨藢榰坁蟙嬂輊翐瀄躑衼汦蹢膱銴駤覟祑馽釞疐"};
const unsigned char PY_mb_zhong []={"中种重钟仲众忠终肿冢衷盅锺種踵終眾塚夂鐘鍾柊伀煄衆螽腫狆尰舯緟蔠穜祌螤彸迚衶籦鈆妐忪蝩堹炂鼨"};
const unsigned char PY_mb_zhou  []={"周州粥洲轴舟咒皱肘昼骤宙洀绉纣胄诌帚週酎妯晝籀軸皺繇碡呪紬侜冑甃啁僽盩喌鈾驟紂箒薵縐詶荮淍胕椆咮謅譸睭翢輈賙婤珘輖鯞銂菷騆徟鵃駲駎"};
const unsigned char PY_mb_zhu   []={"住猪主朱祝著煮注竹驻珠助诸柱逐株筑铸烛诛贮渚竺洙嘱蛛拄豬蛀箸伫諸铢潴茱侏瞩苎炷邾翥杼褚註瘃駐躅硃麈紸佇紵丶笁燭瀦槠术钃疰橥舳築囑誅矚鑄蠋櫧殶属柷貯銖鉒欘窋劯軴灟櫫袾芧砫藸斸墸祩跓鸀篴鋳跙陼尌莇蠾蓫藷磩茿孎蠩跦竚罜嵀馵鴸羜眝蝫泏樦篫鱁絑鮢壴麆"};
const unsigned char PY_mb_zhua  []={"抓爪挝簻檛膼髽"};
const unsigned char PY_mb_zhuai []={"拽跩捙"};
const unsigned char PY_mb_zhuan []={"转赚专砖传撰篆轉馔賺啭專颛顓簨専転磚耑僝漙摶囀瑑剸饌譔塼縳嫥諯膞鱄恮甎篿瑼鄟腞蟤"};
const unsigned char PY_mb_zhuang[]={"装状庄撞壮妆桩幢裝狀莊妝僮荘壯奘湷粧樁戇獞焋梉戆"};
const unsigned char PY_mb_zhui  []={"追坠缀锥椎赘骓墜隹畷甀倕埀惴缒箠綴縋錐硾贅娷騅諈醊餟錣醀腏鵻膇轛鑆"};
const unsigned char PY_mb_zhun  []={"准肫準谆凖迍埻啍窀屯忳諄甽稕訰旽宒綧"};
const unsigned char PY_mb_zhuo  []={"着卓桌捉浊拙灼啄酌濯焯镯斫擢倬茁琢著涿晫剢彴濁浞诼禚斲鷟穱噣缴圴矠啅蹠斀灂椓謶蝃趠諑篧琸汋捔蠗櫡鐲蠿棳梲斮穛罬藋籗硺蓔鵫"};
const unsigned char PY_mb_zi    []={"子字自紫资仔兹姿梓籽滋渍孜姊缁咨呲恣趑吱訾眦淄鲻滓笫孖谘龇呰資髭茲赀耔倳孳菑崰秭辎牸茈嵫粢剚锱沝釨姉胔芓秶諮栥玆漬澬緇鈭訿鎡貲胾薋眥觜鼒璾錙杍胏椔鄑矷穧輜齍紎蓻鶅齜鯔"};
const unsigned char PY_mb_zong  []={"总宗纵踪棕粽综鬃總腙縱偬枞総蹤倧綜傯惣嵕摠豵瘲蓯縦錝鬷昮艐椶愡潀蝬蓗騣惾鑁翪稯猣朡熧"};
const unsigned char PY_mb_zou   []={"走邹揍奏诹驺赱陬鄒鄹偢諏鲰楱掫搊棸齺緅郰菆棷媰騶鯫齱黀"};
const unsigned char PY_mb_zu    []={"组租足族祖阻卒卆組俎鏃崒诅镞菹伜鉏倅爼伹柤綷箤傶哫砠詛珇謯踿鉐怚靻蒩駔趲葅踤"};
const unsigned char PY_mb_zuan  []={"钻赚攥纂鑽欑缵躜纘篹鑚籫躦揝纉"};
const unsigned char PY_mb_zui   []={"最嘴罪醉咀蕞觜枠嶉脧厜晬嫢檇樶睟酔酨噿濢檌嶵嗺鋷璻祽嶊絊欈纗"};
const unsigned char PY_mb_zun   []={"尊遵樽栫鐏鳟拵僔墫撙僎譐罇瀳繜嶟噂袸踆跧鱒銌鷷捘"};
const unsigned char PY_mb_zuo   []={"做作坐左座佐昨唑琢祚嘬凿酢怍鑿柞阼莋笮胙捽蓙鈼稓侳葃筰撮葄繓岝椊"};



/*
*********************************************************************************************************
*	                         拼音检索的结构体数组
*********************************************************************************************************
*/
struct PinYinPCB const PYRankTwoList_a[]=
{
	{"",PY_mb_a},
	{"i",PY_mb_ai},
	{"n",PY_mb_an},
	{"ng",PY_mb_ang},
	{"o",PY_mb_ao}
};


struct PinYinPCB const PYRankTwoList_b[]=
{
	{"a",PY_mb_ba},
	{"ai",PY_mb_bai},
	{"an",PY_mb_ban},
	{"ang",PY_mb_bang},
	{"ao",PY_mb_bao},
    {"en",PY_mb_ben},
	{"eng",PY_mb_beng},
	{"ei",PY_mb_bei},
	{"i",PY_mb_bi},
	{"ian",PY_mb_bian},
	{"iao",PY_mb_biao},
	{"ie",PY_mb_bie},
	{"in",PY_mb_bin},
	{"ing",PY_mb_bing},
	{"o",PY_mb_bo},
	{"u",PY_mb_bu}
};

struct PinYinPCB const PYRankTwoList_c[]=
{
	{"a",PY_mb_ca},
	{"ai",PY_mb_cai},
	{"an",PY_mb_can},
	{"ang",PY_mb_cang},
	{"ao",PY_mb_cao},
	{"e",PY_mb_ce},
    {"en",PY_mb_cen},
	{"eng",PY_mb_ceng},
	{"ha",PY_mb_cha},
	{"hai",PY_mb_chai},
	{"han",PY_mb_chan},
	{"hang",PY_mb_chang},
	{"hao",PY_mb_chao},
	{"he",PY_mb_che},
	{"hen",PY_mb_chen},
	{"heng",PY_mb_cheng},
	{"hi",PY_mb_chi},
	{"hou",PY_mb_chou},
    {"hong",PY_mb_chong},
	{"hu",PY_mb_chu},
    {"hua",PY_mb_chua},
	{"huai",PY_mb_chuai},
	{"huan",PY_mb_chuan},
	{"huang",PY_mb_chuang},
	{"hui",PY_mb_chui},
	{"hun",PY_mb_chun},
	{"huo",PY_mb_chuo},
	{"i",PY_mb_ci},
	{"ong",PY_mb_cong},
	{"ou",PY_mb_cou},
	{"u",PY_mb_cu},
	{"uan",PY_mb_cuan},
	{"ui",PY_mb_cui},
	{"un",PY_mb_cun},
	{"uo",PY_mb_cuo}
};

struct PinYinPCB const PYRankTwoList_d[]=
{
	{"a",PY_mb_da},
	{"ai",PY_mb_dai},
	{"an",PY_mb_dan},
	{"ang",PY_mb_dang},
	{"ao",PY_mb_dao},
	{"e",PY_mb_de},
    {"en",PY_mb_den},
	{"eng",PY_mb_deng},   
	{"ei",PY_mb_dei},
    {"i",PY_mb_di},
    {"ia",PY_mb_dia},
	{"ian",PY_mb_dian},
	{"iao",PY_mb_diao},
	{"ie",PY_mb_die},
	{"ing",PY_mb_ding},
	{"iu",PY_mb_diu},
	{"ong",PY_mb_dong},
	{"ou",PY_mb_dou},
	{"u",PY_mb_du},
	{"uan",PY_mb_duan},
	{"ui",PY_mb_dui},
	{"un",PY_mb_dun},
	{"uo",PY_mb_duo}
};

struct PinYinPCB const PYRankTwoList_e[]=
{
	{"",PY_mb_e},
	{"n",PY_mb_en},
    {"ng",PY_mb_eng},
    {"i",PY_mb_ei},
	{"r",PY_mb_er}
};

struct PinYinPCB const PYRankTwoList_f[]=
{
	{"a",PY_mb_fa},
	{"an",PY_mb_fan},
	{"ang",PY_mb_fang},
	{"ei",PY_mb_fei},
	{"en",PY_mb_fen},
	{"eng",PY_mb_feng},
    {"iao",PY_mb_fiao},
	{"o",PY_mb_fo},
	{"ou",PY_mb_fou},
	{"u",PY_mb_fu}
};

struct PinYinPCB const PYRankTwoList_g[]=
{
	{"a",PY_mb_ga},
	{"ai",PY_mb_gai},
	{"an",PY_mb_gan},
	{"ang",PY_mb_gang},
	{"ao",PY_mb_gao},
	{"e",PY_mb_ge},
	{"ei",PY_mb_gei},
	{"en",PY_mb_gan},
	{"eng",PY_mb_geng},
	{"ong",PY_mb_gong},
	{"ou",PY_mb_gou},
	{"u",PY_mb_gu},
	{"ua",PY_mb_gua},
	{"uai",PY_mb_guai},
	{"uan",PY_mb_guan},
	{"uang",PY_mb_guang},
	{"ui",PY_mb_gui},
	{"un",PY_mb_gun},
	{"uo",PY_mb_guo}
};

struct PinYinPCB const PYRankTwoList_h[]=
{
	{"a",PY_mb_ha},
	{"ai",PY_mb_hai},
	{"an",PY_mb_han},
	{"ang",PY_mb_hang},
	{"ao",PY_mb_hao},
	{"e",PY_mb_he},
	{"ei",PY_mb_hei},
	{"en",PY_mb_hen},
	{"eng",PY_mb_heng},
	{"ong",PY_mb_hong},
	{"ou",PY_mb_hou},
	{"u",PY_mb_hu},
    {"ng",PY_mb_hng},
	{"ua",PY_mb_hua},
	{"uai",PY_mb_huai},
	{"uan",PY_mb_huan},
	{"uang ",PY_mb_huang},
	{"ui",PY_mb_hui},
	{"un",PY_mb_hun},
	{"uo",PY_mb_huo}
};

struct PinYinPCB const PYRankTwoList_i[]=
{
	"",PY_mb_space
};

struct PinYinPCB const PYRankTwoList_j[]=
{
	{"i",PY_mb_ji},
	{"ia",PY_mb_jia},
	{"ian",PY_mb_jian},
	{"iang",PY_mb_jiang},
	{"iao",PY_mb_jiao},
	{"ie",PY_mb_jie},
	{"in",PY_mb_jin},
	{"ing",PY_mb_jing},
	{"iong",PY_mb_jiong},
	{"iu",PY_mb_jiu},
	{"u",PY_mb_ju},
	{"uan",PY_mb_juan},
	{"ue",PY_mb_jue},
	{"un",PY_mb_jun}
};

struct PinYinPCB const PYRankTwoList_k[]=
{
	{"a",PY_mb_ka},
	{"ai",PY_mb_kai},
	{"an",PY_mb_kan},
	{"ang",PY_mb_kang},
	{"ao",PY_mb_kao},
	{"e",PY_mb_ke},
    {"ei",PY_mb_kei},
	{"en",PY_mb_ken},
	{"eng",PY_mb_keng},
	{"ong",PY_mb_kong},
	{"ou",PY_mb_kou},
	{"u",PY_mb_ku},
	{"ua",PY_mb_kua},
	{"uai",PY_mb_kuai},
	{"uan",PY_mb_kuan},
	{"uang",PY_mb_kuang},
	{"ui",PY_mb_kui},
	{"un",PY_mb_kun},
	{"uo",PY_mb_kuo}
};

struct PinYinPCB const PYRankTwoList_l[]=
{
	{"a",PY_mb_la},
	{"ai",PY_mb_lai},
	{"an",PY_mb_lan},
	{"ang",PY_mb_lang},
	{"ao",PY_mb_lao},
	{"e",PY_mb_le},
	{"ei",PY_mb_lei},
	{"eng",PY_mb_leng},
	{"i",PY_mb_li},
    {"ia",PY_mb_lia},
	{"ian",PY_mb_lian},
	{"iang",PY_mb_liang},
	{"iao",PY_mb_liao},
	{"ie",PY_mb_lie},
	{"in",PY_mb_lin},
	{"ing",PY_mb_ling},
	{"iu",PY_mb_liu},
	{"ong",PY_mb_long},
    {"o",PY_mb_lo},
	{"ou",PY_mb_lou},
	{"u",PY_mb_lu},
	{"uan",PY_mb_luan},
	{"ue",PY_mb_lue},
	{"un",PY_mb_lun},
	{"uo",PY_mb_luo},
	{"v",PY_mb_lv}
};

struct PinYinPCB const PYRankTwoList_m[]=
{
    {"",PY_mb_m},
	{"a",PY_mb_ma},
	{"ai",PY_mb_mai},
	{"an",PY_mb_man},
	{"ang",PY_mb_mang},
	{"ao",PY_mb_mao},
	{"e",PY_mb_me},
	{"ei",PY_mb_mei},
	{"en",PY_mb_men},
	{"eng",PY_mb_meng},
	{"i",PY_mb_mi},
	{"ian",PY_mb_mian},
	{"iao",PY_mb_miao},
	{"ie",PY_mb_mie},
	{"in",PY_mb_min},
	{"ing",PY_mb_ming},
	{"iu",PY_mb_miu},
	{"o",PY_mb_mo},
	{"ou",PY_mb_mou},
	{"u",PY_mb_mu}
};

struct PinYinPCB const PYRankTwoList_n[]=
{
    {"",PY_mb_n},
	{"a",PY_mb_na},
	{"ai",PY_mb_nai},
	{"an",PY_mb_nan},
	{"ang",PY_mb_nang},
	{"ao",PY_mb_nao},
	{"e",PY_mb_ne},
	{"ei",PY_mb_nei},
	{"en",PY_mb_nen},
	{"eng",PY_mb_neng},
	{"i",PY_mb_ni},
	{"ian",PY_mb_nian},
	{"iang",PY_mb_niang},
	{"iao",PY_mb_niao},
	{"ie",PY_mb_nie},
	{"in",PY_mb_nin},
	{"ing",PY_mb_ning},
	{"iu",PY_mb_niu},
    {"ou",PY_mb_nou},
	{"ong",PY_mb_nong},
	{"u",PY_mb_nu},
	{"uan",PY_mb_nuan},
	{"ue",PY_mb_nue},
	{"uo",PY_mb_nuo},
	{"v",PY_mb_nv}
};

struct PinYinPCB const PYRankTwoList_o[]=
{
	{"",PY_mb_o},
	{"u",PY_mb_ou}
};

struct PinYinPCB const PYRankTwoList_p[]=
{
	{"a",PY_mb_pa},
	{"ai",PY_mb_pai},
	{"an",PY_mb_pan},
	{"ang",PY_mb_pang},
	{"ao",PY_mb_pao},
	{"ei",PY_mb_pei},
	{"en",PY_mb_pen},
	{"eng",PY_mb_peng},
	{"i",PY_mb_pi},
	{"ian",PY_mb_pian},
	{"iao",PY_mb_piao},
	{"ie",PY_mb_pie},
	{"in",PY_mb_pin},
	{"ing",PY_mb_ping},
	{"o",PY_mb_po},
	{"ou",PY_mb_pou},
	{"u",PY_mb_pu}
};

struct PinYinPCB const PYRankTwoList_q[]=
{
	{"i",PY_mb_qi},
	{"ia",PY_mb_qia},
	{"ian",PY_mb_qian},
	{"iang",PY_mb_qiang},
	{"iao",PY_mb_qiao},
	{"ie",PY_mb_qie},
	{"in",PY_mb_qin},
	{"ing",PY_mb_qing},
	{"iong",PY_mb_qiong},
	{"iu",PY_mb_qiu},
	{"u",PY_mb_qu},
	{"uan",PY_mb_quan},
	{"ue",PY_mb_que},
	{"un",PY_mb_qun}
};

struct PinYinPCB const PYRankTwoList_r[]=
{
	{"an",PY_mb_ran},
	{"ang",PY_mb_rang},
	{"ao",PY_mb_rao},
	{"e",PY_mb_re},
	{"en",PY_mb_ren},
	{"eng",PY_mb_reng},
	{"i",PY_mb_ri},
	{"ong",PY_mb_rong},
	{"ou",PY_mb_rou},
	{"u",PY_mb_ru},
	{"uan",PY_mb_ruan},
	{"ui",PY_mb_rui},
	{"un",PY_mb_run},
	{"uo",PY_mb_ruo}
};

struct PinYinPCB const PYRankTwoList_s[]=
{
	{"a",PY_mb_sa},
	{"ai",PY_mb_sai},
	{"an",PY_mb_san},
	{"ang",PY_mb_sang},
	{"ao",PY_mb_sao},
	{"e",PY_mb_se},
	{"en",PY_mb_sen},
	{"eng",PY_mb_seng},
	{"ha",PY_mb_sha},
	{"hai",PY_mb_shai},
	{"han",PY_mb_shan},
	{"hang ",PY_mb_shang},
	{"hao",PY_mb_shao},
	{"he",PY_mb_she},
	{"hen",PY_mb_shen},
	{"heng",PY_mb_sheng},
	{"hi",PY_mb_shi},
	{"hou",PY_mb_shou},
	{"hu",PY_mb_shu},
	{"hua",PY_mb_shua},
	{"huai",PY_mb_shuai},
	{"huan",PY_mb_shuan},
	{"huang",PY_mb_shuang},
	{"hui",PY_mb_shui},
	{"hun",PY_mb_shun},
	{"huo",PY_mb_shuo},
	{"i",PY_mb_si},
	{"ong",PY_mb_song},
	{"ou",PY_mb_sou},
	{"u",PY_mb_su},
	{"uan",PY_mb_suan},
	{"ui",PY_mb_sui},
	{"un",PY_mb_sun},
	{"uo",PY_mb_suo}
};

struct PinYinPCB const PYRankTwoList_t[]=
{
	{"a",PY_mb_ta},
	{"ai",PY_mb_tai},
	{"an",PY_mb_tan},
	{"ang",PY_mb_tang},
	{"ao",PY_mb_tao},
	{"e",PY_mb_te},
	{"eng",PY_mb_teng},
	{"i",PY_mb_ti},
    {"ei",PY_mb_tei},
	{"ian",PY_mb_tian},
	{"iao",PY_mb_tiao},
	{"ie",PY_mb_tie},
	{"ing",PY_mb_ting},
	{"ong",PY_mb_tong},
	{"ou",PY_mb_tou},
	{"u",PY_mb_tu},
	{"uan",PY_mb_tuan},
	{"ui",PY_mb_tui},
	{"un",PY_mb_tun},
	{"uo",PY_mb_tuo}
};

struct PinYinPCB const PYRankTwoList_u[]=
{
	{"",PY_mb_space}
};

struct PinYinPCB const PYRankTwoList_v[]=
{
	{"",PY_mb_space}
};

struct PinYinPCB const PYRankTwoList_w[]=
{
	{"a",PY_mb_wa},
	{"ai",PY_mb_wai},
	{"an",PY_mb_wan},
	{"ang",PY_mb_wang},
	{"ei",PY_mb_wei},
	{"en",PY_mb_wen},
	{"eng",PY_mb_weng},
	{"o",PY_mb_wo},
	{"u",PY_mb_wu}
};

struct PinYinPCB const PYRankTwoList_x[]=
{
	{"i",PY_mb_xi},
	{"ia",PY_mb_xia},
	{"ian",PY_mb_xian},
	{"iang",PY_mb_xiang},
	{"iao",PY_mb_xiao},
	{"ie",PY_mb_xie},
	{"in",PY_mb_xin},
	{"ing",PY_mb_xing},
	{"iong",PY_mb_xiong},
	{"iu",PY_mb_xiu},
	{"u",PY_mb_xu},
	{"uan",PY_mb_xuan},
	{"ue",PY_mb_xue},
	{"un",PY_mb_xun}
};

struct PinYinPCB const PYRankTwoList_y[]=
{
	{"a",PY_mb_ya},
	{"an",PY_mb_yan},
	{"ang",PY_mb_yang},
	{"ao",PY_mb_yao},
	{"e",PY_mb_ye},
	{"i",PY_mb_yi},
	{"in",PY_mb_yin},
	{"ing",PY_mb_ying},
	{"o",PY_mb_yo},
	{"ong",PY_mb_yong},
	{"ou",PY_mb_you},
	{"u",PY_mb_yu},
	{"uan",PY_mb_yuan},
	{"ue",PY_mb_yue},
	{"un",PY_mb_yun}
};

struct PinYinPCB const PYRankTwoList_z[]=
{
	{"a",PY_mb_za},
	{"ai",PY_mb_zai},
	{"an",PY_mb_zan},
	{"ang",PY_mb_zang},
	{"ao",PY_mb_zao},
	{"e",PY_mb_ze},
	{"ei",PY_mb_zei},
	{"en",PY_mb_zen},
	{"eng",PY_mb_zeng},
	{"ha",PY_mb_zha},
	{"hai",PY_mb_zhai},
	{"han",PY_mb_zhan},
	{"hang",PY_mb_zhang},
	{"hao",PY_mb_zhao},
	{"he",PY_mb_zhe},
	{"hen",PY_mb_zhen},
	{"heng",PY_mb_zheng},
	{"hi",PY_mb_zhi},
	{"hong",PY_mb_zhong},
	{"hou",PY_mb_zhou},
	{"hu",PY_mb_zhu},
	{"hua",PY_mb_zhua},
	{"huai",PY_mb_zhuai},
	{"huan",PY_mb_zhuan},
	{"huang",PY_mb_zhuang},
	{"hui",PY_mb_zhui},
	{"hun",PY_mb_zhun},
	{"huo",PY_mb_zhuo},
	{"i",PY_mb_zi},
	{"ong",PY_mb_zong},
	{"ou",PY_mb_zou},
	{"u",PY_mb_zu},
	{"uan",PY_mb_zuan},
	{"ui",PY_mb_zui},
	{"un",PY_mb_zun},
	{"uo",PY_mb_zuo}
};

struct PinYinPCB const PYRankTwoList_end[]=
{
	"",	PY_mb_space
};

/*
*********************************************************************************************************
*	                         拼音检索的结构体数组的指针
*********************************************************************************************************
*/
struct PinYinPCB const *PYRankThreeList[]=
{  
	PYRankTwoList_a,
	PYRankTwoList_b,
	PYRankTwoList_c,
	PYRankTwoList_d,
	PYRankTwoList_e,
	PYRankTwoList_f,
	PYRankTwoList_g,
	PYRankTwoList_h,
	PYRankTwoList_i,
	PYRankTwoList_j,
	PYRankTwoList_k,
	PYRankTwoList_l,
	PYRankTwoList_m,
	PYRankTwoList_n,
	PYRankTwoList_o,
	PYRankTwoList_p,
	PYRankTwoList_q,
	PYRankTwoList_r,
	PYRankTwoList_s,
	PYRankTwoList_t,
	PYRankTwoList_u,
	PYRankTwoList_v,
	PYRankTwoList_w,
	PYRankTwoList_x,
	PYRankTwoList_y,
	PYRankTwoList_z,
	PYRankTwoList_end
};

/*
*********************************************************************************************************
*	函 数 名: PYSearch
*	功能说明: 检索拼音的函数,使用很简单，只需输入拼音就行返回这个拼音对应的汉字个数。
*	形    参：msg   拼音字符串
*             num   返回此拼音对应的汉字个数
*	返 回 值: 如果这个拼音存在汉字与其对应，返回检索到的汉字的指针，如果没有，返回空指针
*********************************************************************************************************
*/										
char *PYSearch(unsigned char *msg , unsigned int *num)
{
	unsigned char count=0;
	unsigned char i=0;
	/*
		例如‘b'，以读音为’b'开头的所有汉字
		b'的下一个‘c'，以读音为’c'开头的所有汉字
		通过这比较两个数据的大小可以得到’b'的所有的元音+辅音
	*/
	struct PinYinPCB const *headpcb; 		
	struct PinYinPCB const *tailpcb;		
											
	while(msg[count] != 0) count++;			  /* 计算检索的拼音的字节数     */	
	
	for(i = 0; i < count; i++)				  /* 检查检索的拼音是否符合规范 */
	{
		if((msg[i] >= 'a') && (msg[i]<='z'))  /* 在这个区间的数据有效       */
		{
			continue;
		}								   	  /* 输入的数据非法，返回空指针 */
		else
		{
		    return '\0';
		}
	}
	
	/* 输入的数据非法，返回空指针 */
	if((*msg == 'i')||(*msg == 'u')||(*msg == 'v')||(*msg == '\0'))	
	{
	    return '\0';
	}
	
	/* 以读音为*msg开头的所有拼音检索 */
	tailpcb = PYRankThreeList[*msg - 'a'];	
	
 	/* 以读音为*msg+1开头的所有拼音检索 */
	headpcb = PYRankThreeList[*msg - 'a' + 1];
	
	for(; tailpcb < headpcb; tailpcb++)
	{
		
		/* 检查输入的拼音与检索结构体中的拼音是否一致 */
		for(i = 0; i < count - 1; i++)				
		{	
			if(*((*tailpcb).PYSerList+i) != msg[i+1])
			{
				break;
			}
		}
		
		/* 一致，返回指针 */
		if(i == (count - 1))	 					
		{
			/* 特别注意下面这个函数，因为UTF-8编码格式的汉字是3个字节 */
			*num = strlen((const char *)(*tailpcb).PYRankOneList) / 3;
			return (char *)(*tailpcb).PYRankOneList;
		}

	}
	
	/* 否者返回空的指针 */
	return '\0';								
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
