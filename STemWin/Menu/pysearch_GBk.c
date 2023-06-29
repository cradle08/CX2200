/*
*********************************************************************************************************
*	                                  
*	ģ������ : ȫƴ�����뷨
*	˵    �� : �ⲿ�ֺ��ֱ�ͼ��������������磬ʹ�ñ������뱣֤���ļ���������UCS-2 Little��ʽ
*********************************************************************************************************
*/


#include "pysearch.h"
#include "string.h"


/*
*********************************************************************************************************
*	                         �����б�
*********************************************************************************************************
*/
const unsigned char PY_mb_space []={""};
const unsigned char PY_mb_a     []={"������߹�����"}; 
const unsigned char PY_mb_ai    []={"���������������������������������������������"};
const unsigned char PY_mb_an    []={"���������������������������������������"};
const unsigned char PY_mb_ang   []={"��������"};
const unsigned char PY_mb_ao    []={"�������°��ð��������������������������������"};
const unsigned char PY_mb_ba    []={"�Ѱ˰ɰְΰհϰͰŰǰӰ԰Ȱаʰ̰Ұ������������"};
const unsigned char PY_mb_bai   []={"�ٰװڰܰذݰ۲���������"};
const unsigned char PY_mb_ban   []={"��������߰����������������"};
const unsigned char PY_mb_bang  []={"���������������������"};
const unsigned char PY_mb_bao   []={"�����������������������ٱ����������������������������"};
const unsigned char PY_mb_bei   []={"�������������������������������������������"};
const unsigned char PY_mb_ben   []={"�����������������"};
const unsigned char PY_mb_beng  []={"�ı��±��Ű������"};
const unsigned char PY_mb_bi    []={"�ȱʱձǱ̱رܱƱϱ۱˱ɱڱͱұױٱαбӱֱݱѱ�������޵ݩ����ذ���������������������������������������������"};
const unsigned char PY_mb_bian  []={"�߱�������ޱ���������������������������"};
const unsigned char PY_mb_biao  []={"���������������������"};
const unsigned char PY_mb_bie   []={"�������"};
const unsigned char PY_mb_bin   []={"������������������������������"};
const unsigned char PY_mb_bing  []={"����������������������������"};
const unsigned char PY_mb_bo    []={"����������������������������������ز����������ǲ�������������ެ�������"};
const unsigned char PY_mb_bu    []={"�������������������������������������߲"};
const unsigned char PY_mb_ca    []={"��������"};
const unsigned char PY_mb_cai   []={"�²ŲĲƲòɲʲǲȲ˲�"};
const unsigned char PY_mb_can   []={"�ϲв��βҲѲͲ����������"};
const unsigned char PY_mb_cang  []={"�زֲײղ���"};
const unsigned char PY_mb_cao   []={"�ݲٲܲ۲��������"};
const unsigned char PY_mb_ce    []={"���߲����"};
const unsigned char PY_mb_cen   []={"����"};
const unsigned char PY_mb_ceng  []={"�������"};
const unsigned char PY_mb_cha   []={"�����������ɲ��������������������"};
const unsigned char PY_mb_chai  []={"�˲���������٭"};
const unsigned char PY_mb_chan  []={"��������������������������������������������������"};
const unsigned char PY_mb_chang []={"���������������������������������������������������"};
const unsigned char PY_mb_chao  []={"�����������������˴³���������"};
const unsigned char PY_mb_che   []={"�����������߳����"};
const unsigned char PY_mb_chen  []={"�óƳ����������³ĳȳ��������������������"};
const unsigned char PY_mb_cheng []={"�ɳ�ʢ�ųƳǳ̳ʳϳͳѳҳγȳ�����������������������ة�"};
const unsigned char PY_mb_chi   []={"�Գ߳ٳس�ճ�ݳֳܳ�޳ڳ۳��������ܯ����������������������߳������"};
const unsigned char PY_mb_chong []={"���س����ӿ������������"};
const unsigned char PY_mb_chou  []={"���������������ٱ�����"};
const unsigned char PY_mb_chu   []={"�������������������������������������ۻ���ء����������"};
const unsigned char PY_mb_chuai []={"�����������"};
const unsigned char PY_mb_chuan []={"������������������������"};
const unsigned char PY_mb_chuang[]={"��������������"};
const unsigned char PY_mb_chui  []={"����������׵����"};
const unsigned char PY_mb_chun  []={"����������������ݻ��"};
const unsigned char PY_mb_chuo  []={"����������"};
const unsigned char PY_mb_ci    []={"�δ˴ʴɴȴƴŴǴ̴��ŴôͲ�����������"};
const unsigned char PY_mb_cong  []={"�ӴԴдҴϴ������������"};
const unsigned char PY_mb_cou   []={"������"};
const unsigned char PY_mb_cu    []={"�ִ״ش����������������"};
const unsigned char PY_mb_cuan  []={"�ܴڴ���ߥ������"};
const unsigned char PY_mb_cui   []={"�ߴ�ݴ�޴�˥������������"};
const unsigned char PY_mb_cun   []={"���������"};
const unsigned char PY_mb_cuo   []={"������������������������"};
const unsigned char PY_mb_da    []={"����������������������������"};
const unsigned char PY_mb_dai   []={"�������������������������߾�����߰�����ܤ"};
const unsigned char PY_mb_dan   []={"������������������������������ʯ��������������������"};
const unsigned char PY_mb_dang  []={"����������������������"};
const unsigned char PY_mb_dao   []={"���������������������������������߶"};
const unsigned char PY_mb_de    []={"�ĵصõµ��"};
const unsigned char PY_mb_deng  []={"�ȵƵ˵ǳεɵʵ������������"};
const unsigned char PY_mb_di    []={"�صڵ׵͵еֵε۵ݵյܵ޵̵ĵ���ѵϵҵԵ���ۡ��ڮ��������ص����ݶ����������"};
const unsigned char PY_mb_dia	[]={"��"};
const unsigned char PY_mb_dian  []={"�������ߵ�������������������������"};
const unsigned char PY_mb_diao  []={"�����������������������"};
const unsigned char PY_mb_die   []={"���������������ܦ������������"};
const unsigned char PY_mb_ding  []={"������������������������������������"};
const unsigned char PY_mb_diu   []={"����"};
const unsigned char PY_mb_dong  []={"����������������������������������"};
const unsigned char PY_mb_dou   []={"������������������������"};
const unsigned char PY_mb_du    []={"���ȶ��ɶ¶��ǶƶĶöŶ������ʶ��������������ܶ"};
const unsigned char PY_mb_duan  []={"�ζ̶϶˶Ͷ������"};
const unsigned char PY_mb_dui   []={"�ԶӶѶҶ�������"};
const unsigned char PY_mb_dun   []={"�ֶٶ׶նض۶ܶڶݲ��������������"};
const unsigned char PY_mb_duo   []={"����������޶��Զȶ��������������"};
const unsigned char PY_mb_e     []={"��Ŷ������ﰢ������������������������ݭ��������ج����"};
const unsigned char PY_mb_en    []={"������"};
const unsigned char PY_mb_er    []={"���������������������٦����"};
//const unsigned char PY_mb_fa    []={"��������������������"};
const unsigned char PY_mb_fa    []=	 {"��������������������"};
const unsigned char PY_mb_fan   []={"���������������������������������������ެ�����"};
const unsigned char PY_mb_fang  []={"�ŷ����ķ����÷·�����������������"};
const unsigned char PY_mb_fei   []={"�ǷɷʷѷηϷ˷ͷзƷ̷�������������������������������"};
const unsigned char PY_mb_fen   []={"�ַݷҷ۷طܷ߷׷޷�ӷٷԷշ������������"};
const unsigned char PY_mb_feng  []={"�����������������ٺۺ������"};
const unsigned char PY_mb_fo    []={"��"};
const unsigned char PY_mb_fou   []={"���"};
const unsigned char PY_mb_fu    []={"�����������������������������������򸸷����󸳸���������������������������������и����������������������������������������������������������ݳ���������߻ۮܽ��"};
const unsigned char PY_mb_ga    []={"����и¿�����٤���������"};
const unsigned char PY_mb_gai   []={"�øĸǸŸƽ�����ؤ����"};
const unsigned char PY_mb_gan   []={"�ϸɸиҸ͸ʸθ̸˸Ӹ�����������ߦ�����������"};
const unsigned char PY_mb_gang  []={"�ոָٸ۸׸ڸܸԸؿ�����"};
const unsigned char PY_mb_gao   []={"�߸����ݸ����۬ھ���غ�����޻"};
const unsigned char PY_mb_ge    []={"�������������￩���������ٺϸ��������ܪ���������ت���������"};
const unsigned char PY_mb_gei   []={"��"};
const unsigned char PY_mb_gen   []={"������ݢب��"};
const unsigned char PY_mb_geng  []={"�����������������������"};
const unsigned char PY_mb_gong  []={"���������������������������������������"};
const unsigned char PY_mb_gou   []={"��������������������������������ڸ��������"};
const unsigned char PY_mb_gu    []={"�ŹɹĹȹʹ¹��ù˹̹͹����ǹ����Ƽֹ��������������������������ڬ�����������������"};
const unsigned char PY_mb_gua   []={"�ҹιϹѹй����������ڴ"};
const unsigned char PY_mb_guai  []={"�ֹչ���"};
const unsigned char PY_mb_guan  []={"�عܹٹ۹ݹ߹޹�ڹ��������ݸ����������"};
const unsigned char PY_mb_guang []={"������������"};
const unsigned char PY_mb_gui   []={"��������������������Ȳ���������������������"};
const unsigned char PY_mb_gun   []={"�������������"};
const unsigned char PY_mb_guo   []={"�����������������������������������"};
const unsigned char PY_mb_ha    []={"����Ϻ��"};
const unsigned char PY_mb_hai   []={"�������Ⱥ���������������"};
const unsigned char PY_mb_han   []={"����������������������������������������������������������"};
const unsigned char PY_mb_hang  []={"���ﺽ�������������"};
const unsigned char PY_mb_hao   []={"�úźƺ����º����ĺѸ�������������޶"};
const unsigned char PY_mb_he    []={"�ͺȺϺӺ̺˺κǺɺغպֺк׺ʺѺҺ����������������ڭ����"};
const unsigned char PY_mb_hei   []={"�ں�"};
const unsigned char PY_mb_hen   []={"�ܺݺ޺�"};
const unsigned char PY_mb_heng  []={"���ߺ��������޿"};
const unsigned char PY_mb_hong  []={"����������ڧ��ޮ��ް��ݦ��"};
const unsigned char PY_mb_hou   []={"������������ܩ����������"};
const unsigned char PY_mb_hu    []={"��������������������������������Ϸ�˺ͺ�������������������������������������������������"};
const unsigned char PY_mb_hua   []={"���������������������������"};
const unsigned char PY_mb_huai  []={"��������������"};
const unsigned char PY_mb_huan  []={"���������������û»������������ߧ����ۨ���ۼ����������"};
const unsigned char PY_mb_huang []={"�ƻŻλĻɻ˻ʻѻ̻Ȼǻлͻ�������������"};
const unsigned char PY_mb_hui   []={"�ػ�һ�ӻ�Իٻڻݻ޻ջֻ�ۻ߻׻仲�ܻ�������ޥ����������ڶ�����������"};
const unsigned char PY_mb_hun   []={"���������������ڻ"};
const unsigned char PY_mb_huo   []={"�������ͻ���������������޽߫������"};
const unsigned char PY_mb_ji    []={"�������ȼ��������Ǽ����Ƽ������ļ�ϵ���������ʻ������������������������������������������������¼ü����������˼̸���������������������ڵ��������ߴ����������������������آ��ܸ٥�������ު����"};
const unsigned char PY_mb_jia   []={"�ҼӼټۼܼ׼Ѽмμݼ޼ϼԼռؼ��Ѽ�������������������٤��������ۣ����"};
const unsigned char PY_mb_jian  []={"��������������潨��������߼��齡�������������ǳ���������𽢼���뽤��������������������������������������������������"};
const unsigned char PY_mb_jiang []={"������������������������ǿ��������������������"};
const unsigned char PY_mb_jiao  []={"�нŽ��ǽ̽Ͻɾ���������У�����ƽ��ý������νѽ����Ƚ½ͽĽ���ܴ������������������ٮ������"};
const unsigned char PY_mb_jie   []={"�ӽڽֽ�Խؽ�����ҽ��׽ٽ�߽���ۿ��ս۽ܽݽ�޼��������������������ڦ����ڵ"};
const unsigned char PY_mb_jin   []={"�����������ﾡ�������������������������������ݣ�������������"};
const unsigned char PY_mb_jing  []={"�������������������������������������������������������������������ݼ���������"};
const unsigned char PY_mb_jiong []={"��������"};
const unsigned char PY_mb_jiu   []={"�;žƾɾþ��Ⱦ��˾��¾Ǿʾ��ľ̾�������������������"};
const unsigned char PY_mb_ju    []={"��پ޾־߾���Ӿ۾оվؾھܾ�ϾѾ��Ҿݹ�۾㳵�׾Ҿ��������������������������������������������ڪ��"};
const unsigned char PY_mb_juan  []={"��Ȧ�������۲�������������"};
const unsigned char PY_mb_jue   []={"�������Ǿ����������������������������������ާ����������"};
const unsigned char PY_mb_jun   []={"�������������꿢����������������"};
const unsigned char PY_mb_ka    []={"��������������"};
const unsigned char PY_mb_kai   []={"�������������������������"};
const unsigned char PY_mb_kan   []={"��������Ƕ���������٩�ݨ��"};
const unsigned char PY_mb_kang  []={"��������������������"};
const unsigned char PY_mb_kao   []={"����������������"};
const unsigned char PY_mb_ke    []={"�ȿɿ˿ÿƿſ̿οͿǿʿ��¿Ŀ��������������������������������"};
const unsigned char PY_mb_ken   []={"�Ͽпҿ���"};
const unsigned char PY_mb_keng  []={"�ӿ��"};
const unsigned char PY_mb_kong  []={"�տ׿ؿ�������"};
const unsigned char PY_mb_kou   []={"�ڿۿٿ�ޢ������ߵ"};
const unsigned char PY_mb_ku    []={"�޿��ݿ�߿������ܥ�"};
const unsigned char PY_mb_kua   []={"������٨"};
const unsigned char PY_mb_kuai  []={"�����������ۦ����"};
const unsigned char PY_mb_kuan  []={"�����"};
const unsigned char PY_mb_kuang []={"�����������ڿ������ڲ������"};
const unsigned char PY_mb_kui   []={"�������������������������������������������ظ������"};
const unsigned char PY_mb_kun   []={"��������������������"};
const unsigned char PY_mb_kuo   []={"��������������"};
const unsigned char PY_mb_la    []={"���������������������������"};
const unsigned char PY_mb_lai   []={"���������������������"};
const unsigned char PY_mb_lan   []={"����������������������������������������"};
const unsigned char PY_mb_lang  []={"������������������ݹ������"};
const unsigned char PY_mb_lao   []={"���������������������������������������"};
const unsigned char PY_mb_le    []={"����������߷����"};
const unsigned char PY_mb_lei   []={"��������������������������ڳ������������"};
const unsigned char PY_mb_leng  []={"������ܨ�"};
const unsigned char PY_mb_li    []={"��������������������������������������������������������������������ٳٵ۪����ݰ���������������������ت߿������������޼���������������������������"};
const unsigned char PY_mb_lia	[]={"��"};
const unsigned char PY_mb_lian  []={"������������������������������������������������"};
const unsigned char PY_mb_liang []={"������������������������ܮ�������ݹ"};
const unsigned char PY_mb_liao  []={"����������������������������ޤ����������"};
const unsigned char PY_mb_lie   []={"�������������������������"};
const unsigned char PY_mb_lin   []={"�����������������������������������������������"};
const unsigned char PY_mb_ling  []={"��������������������������������������۹��������������"};
const unsigned char PY_mb_liu   []={"����������������������µ½����������������"};
const unsigned char PY_mb_long  []={"��£����¡¢Ū����¤���������������"};
const unsigned char PY_mb_lou   []={"¥§©ª¶¦¨�����������������"};
const unsigned char PY_mb_lu    []={"·¶¼¹½¯¬³±«­®µ°��²¸¾º»´��������ޤ�������������������"};
const unsigned char PY_mb_luan  []={"��������������������"};
const unsigned char PY_mb_lue   []={"�����"};
const unsigned char PY_mb_lun   []={"����������������"};
const unsigned char PY_mb_luo   []={"�������������������������翩���������������������������"};
const unsigned char PY_mb_lv    []={"������¿���������������������������������"};
const unsigned char PY_mb_ma    []={"������������Ĩ������Ħ��������"};
const unsigned char PY_mb_mai   []={"������������۽��ݤ"};
const unsigned char PY_mb_man   []={"������������������á���ܬ�����������"};
const unsigned char PY_mb_mang  []={"æâäçãå��������"};
const unsigned char PY_mb_mao   []={"ëðñèìîòïóíêé���������������������"};
const unsigned char PY_mb_me    []={"ô��"};
const unsigned char PY_mb_mei   []={"ûÿúþ��ø��öùõü÷����ý������ĭ����������ݮ�����"};
const unsigned char PY_mb_men   []={"�������������"};
const unsigned char PY_mb_meng  []={"����������������å������������������ޫ"};
const unsigned char PY_mb_mi    []={"��������������������������������������������������������"};
const unsigned char PY_mb_mian  []={"������������������������������"};
const unsigned char PY_mb_miao  []={"������������������������������"};
const unsigned char PY_mb_mie   []={"���������ؿ"};
const unsigned char PY_mb_min   []={"������������������������������"};
const unsigned char PY_mb_ming  []={"��������������ڤ���������"};
const unsigned char PY_mb_miu   []={"����"};
const unsigned char PY_mb_mo    []={"��ĥĨĩĤīûĪĬħģĦġĮİ��ĭ����ðį�������Ժ���Ģ�����������������"};
const unsigned char PY_mb_mou   []={"ĳıĲ������ٰ����"};
const unsigned char PY_mb_mu    []={"ľĸĶĻĿĹ��Ĳģ��ĺĵĴļĽ��ķ�������������"};
const unsigned char PY_mb_na    []={"������������������������"};
const unsigned char PY_mb_nai   []={"��������������ܵ��ؾ٦"};
const unsigned char PY_mb_nan   []={"����������������"};
const unsigned char PY_mb_nang  []={"��������߭"};
const unsigned char PY_mb_nao   []={"����������ث������������"};
const unsigned char PY_mb_ne    []={"��������ګ"};
const unsigned char PY_mb_nei   []={"��������"};
const unsigned char PY_mb_nen   []={"���"};
const unsigned char PY_mb_neng  []={"��"};
const unsigned char PY_mb_ni    []={"���������������������������������٣�������"};
const unsigned char PY_mb_nian  []={"��������������ճإ���������"};
const unsigned char PY_mb_niang []={"����"};
const unsigned char PY_mb_niao  []={"������������"};
const unsigned char PY_mb_nie   []={"������������������������ؿ"};
const unsigned char PY_mb_nin   []={"���"};
const unsigned char PY_mb_ning  []={"š��������Ţ�������"};
const unsigned char PY_mb_niu   []={"ţŤŦť�������"};
const unsigned char PY_mb_nong  []={"ŪŨũŧ��ٯ"};
const unsigned char PY_mb_nu    []={"ŭŬū��������"};
const unsigned char PY_mb_nuan  []={"ů"};
const unsigned char PY_mb_nue   []={"Űű��"};
const unsigned char PY_mb_nuo   []={"ŲŵųŴ���������"};
const unsigned char PY_mb_nv    []={"Ů�����"};
const unsigned char PY_mb_o     []={"Ŷ���"};
const unsigned char PY_mb_ou    []={"żŻŷźŸ��ŽŹ���ک��"};
const unsigned char PY_mb_pa    []={"����ſž�Ұ�������������"};
const unsigned char PY_mb_pai   []={"����������������ٽ��"};
const unsigned char PY_mb_pan   []={"���������������ͷ������������������"};
const unsigned char PY_mb_pang  []={"���������Ұ���������"};
const unsigned char PY_mb_pao   []={"������������������������"};
const unsigned char PY_mb_pei   []={"�����������������������������"};
const unsigned char PY_mb_pen   []={"������"};
const unsigned char PY_mb_peng  []={"������������������������������ܡ���"};
const unsigned char PY_mb_pi    []={"��Ƥ��ƥ������ƨƢƧƣƦ������ơƩ������ا�������ߨۯ������ܱ�����������������������"};
const unsigned char PY_mb_pian  []={"Ƭƪƭƫ�����������������"};
const unsigned char PY_mb_piao  []={"ƱƮƯư���ݳ�������������"};
const unsigned char PY_mb_pie   []={"ƳƲ���"};
const unsigned char PY_mb_pin   []={"ƷƶƸƴƵ�������"};
const unsigned char PY_mb_ping  []={"ƽƾƿ����ƹƼƻƺ�������ٷ"};
const unsigned char PY_mb_po    []={"�����������Ȳ����ӷ����������۶�������"};
const unsigned char PY_mb_pou   []={"������"};
const unsigned char PY_mb_pu    []={"�����׸����������������������ֱ��ձ����������������W"};
const unsigned char PY_mb_qi    []={"�������������������������������������������������������������������������������������ݽޭ���������ܻ��ؽ��ٹ��������������������������������"};
const unsigned char PY_mb_qia   []={"ǡ����Ǣ������"};
const unsigned char PY_mb_qian  []={"ǰǮǧǣǳǩǷǦǶǥǨǯǬǴǫǱǸ��ǤǲǭǵǪ���������ٻ������ݡ����ܷ�����������"};
const unsigned char PY_mb_qiang []={"ǿǹǽ��ǻǺǼǾ������������������������"};
const unsigned char PY_mb_qiao  []={"�����������¿�����������ȸ������������������������ڽ��"};
const unsigned char PY_mb_qie   []={"������������ۧ���������٤��"};
const unsigned char PY_mb_qin   []={"����������������������������������������������"};
const unsigned char PY_mb_qing  []={"��������������������������������������������������"};
const unsigned char PY_mb_qiong []={"�����������������"};
const unsigned char PY_mb_qiu   []={"���������������������������������ٴ�������������"};
const unsigned char PY_mb_qu    []={"ȥȡ��Ȣ������Ȥ��������ȣ���޾��ޡ��۾ڰ�����������������������"};
const unsigned char PY_mb_quan  []={"ȫȨȰȦȭȮȪȯȧȬȩ�����ڹ����������"};
const unsigned char PY_mb_que   []={"ȴȱȷȸȳȵȲȶ�����"};
const unsigned char PY_mb_qun   []={"Ⱥȹ����"};
const unsigned char PY_mb_ran   []={"ȾȼȻȽ������"};
const unsigned char PY_mb_rang  []={"����ȿ�������"};
const unsigned char PY_mb_rao   []={"�����������"};
const unsigned char PY_mb_re    []={"��������"};
const unsigned char PY_mb_ren   []={"���������������������������������"};
const unsigned char PY_mb_reng  []={"����"};
const unsigned char PY_mb_ri    []={"��"};
const unsigned char PY_mb_rong  []={"������������������������������"};
const unsigned char PY_mb_rou   []={"������������"};
const unsigned char PY_mb_ru    []={"������������������������������޸����"};
const unsigned char PY_mb_ruan  []={"������"};
const unsigned char PY_mb_rui   []={"������������ި"};
const unsigned char PY_mb_run   []={"����"};
const unsigned char PY_mb_ruo   []={"����ټ��"};
const unsigned char PY_mb_sa    []={"����������ئ���"};
const unsigned char PY_mb_sai   []={"������˼����"};
const unsigned char PY_mb_san   []={"��ɢɡ���������"};
const unsigned char PY_mb_sang  []={"ɣɥɤ�����"};
const unsigned char PY_mb_sao   []={"ɨɩɦɧ��ܣ��������"};
const unsigned char PY_mb_se    []={"ɫɬɪ������"};
const unsigned char PY_mb_sen   []={"ɭ"};
const unsigned char PY_mb_seng  []={"ɮ"};
const unsigned char PY_mb_sha   []={"ɱɳɶɴɵɰɲɯ��ɷɼ����������������"};
const unsigned char PY_mb_shai  []={"ɹɸɫ��"};
const unsigned char PY_mb_shan  []={"ɽ��������ɼɾɿ��ɺ����դɻ���������������������۷�������������ڨ������"};
const unsigned char PY_mb_shang []={"���������������������������"};
const unsigned char PY_mb_shao  []={"����������������������������ۿ��������"};
const unsigned char PY_mb_she   []={"�������������������������������������"};
const unsigned char PY_mb_shen  []={"�����������������������������ʲ��������ݷڷ�������������"};
const unsigned char PY_mb_sheng []={"��ʡʣ������ʤʢʥ����������������"};
const unsigned char PY_mb_shi   []={"��ʹʮʱ������ʯʦ��ʷʽʶʭʸʰʺʻʼ����ʾʿ��������������ʲֳ������ʧ������������ʨʳ��ʴ��ʵʩʪʫʬ��ݪ����������������������߱���"};
const unsigned char PY_mb_shou  []={"���������������������������"};
const unsigned char PY_mb_shu   []={"��������������������������������������������������ˡ�������������������"};
const unsigned char PY_mb_shua  []={"ˢˣ�"};
const unsigned char PY_mb_shuai []={"ˤ˦��˧˥�"};
const unsigned char PY_mb_shuan []={"˨˩����"};
const unsigned char PY_mb_shuang[]={"˫˪ˬ����"};
const unsigned char PY_mb_shui  []={"ˮ˭˯˰˵"};
const unsigned char PY_mb_shun  []={"˳˱˲˴"};
const unsigned char PY_mb_shuo  []={"˵��˶˸˷����������"};
const unsigned char PY_mb_si    []={"����˿˺��˽˻˼��˾˹ʳ�Ų���������������������������������������ٹ"};
const unsigned char PY_mb_song  []={"�����������������ݿ�����ڡ����"};
const unsigned char PY_mb_sou   []={"��������������޴�����������"};
const unsigned char PY_mb_su    []={"�����������������������������������������"};
const unsigned char PY_mb_suan  []={"�������"};
const unsigned char PY_mb_sui   []={"���������������������������������ݴ�"};
const unsigned char PY_mb_sun   []={"�������ݥ�����"};
const unsigned char PY_mb_suo   []={"��������������ɯ�����������������"};
const unsigned char PY_mb_ta    []={"������̤������̡̢̣�����������������"};
const unsigned char PY_mb_tai   []={"̧̨̫̬̥̦̩̪̭������߾����޷ۢ��"};
const unsigned char PY_mb_tan   []={"̸̲̾̽��̶̵̷̴̼̯̰̳̺̹̱̮̻̿۰������������"};
const unsigned char PY_mb_tang  []={"�����������������������������������������������"};
const unsigned char PY_mb_tao   []={"����������������������ػ������߶��"};
const unsigned char PY_mb_te    []={"������߯"};
const unsigned char PY_mb_teng  []={"����������"};
const unsigned char PY_mb_ti    []={"���������������������������������������������"};
const unsigned char PY_mb_tian  []={"����������������������������"};
const unsigned char PY_mb_tiao  []={"����������������������٬�������"};
const unsigned char PY_mb_tie   []={"����������"};
const unsigned char PY_mb_ting  []={"��ͣͦ��ͤͧͥ͢��͡���������������"};
const unsigned char PY_mb_tong  []={"ͬͨʹͭͰͲͱͳͯͮͩͫ����ͪ�����������١����"};
const unsigned char PY_mb_tou   []={"ͷ͵͸Ͷ����"};
const unsigned char PY_mb_tu    []={"��ͼ��Ϳ��ͺͻͽ͹;��ܢݱ������"};
const unsigned char PY_mb_tuan  []={"���������"};
const unsigned char PY_mb_tui   []={"��������������߯"};
const unsigned char PY_mb_tun   []={"�������ζ��������"};
const unsigned char PY_mb_tuo   []={"������������������������ر٢����������������������"};
const unsigned char PY_mb_wa    []={"�����������ݰ��������"};
const unsigned char PY_mb_wai   []={"������"};
const unsigned char PY_mb_wan   []={"��������������������������������������ݸ�����ܹ���������"};
const unsigned char PY_mb_wang  []={"��������������������â�������"};
const unsigned char PY_mb_wei   []={"ΪλδΧιθ΢ζβα��ΰ��ΣΥίκΨάηΩΤΡενξΫγοΦήέμ�������������������������������������������ޱ��"};
const unsigned char PY_mb_wen   []={"�����������������������������"};
const unsigned char PY_mb_weng  []={"��������޳"};
const unsigned char PY_mb_wo    []={"������������������������ݫ�����"};
const unsigned char PY_mb_wu    []={"������������������������������������������������������������������أ���������������������������������������"};
const unsigned char PY_mb_xi    []={"��ϴϸ��ϷϵϲϯϡϪϨ��ϥϢϮϧϰ��ϦϤ����ϣϭ������ϱ��ϳϩ����϶��ϫϬ����������������������������������������������ۭ����������������ݾ������"};
const unsigned char PY_mb_xia   []={"������ϿϺϹϼ��ϻ��Ͻ��Ͼ�����������������"};
const unsigned char PY_mb_xian  []={"����������������������������������������������������ϳ����޺���ݲ���������������������������"};
const unsigned char PY_mb_xiang []={"���������������������������ή������������ܼ��������������"};
const unsigned char PY_mb_xiao  []={"СЦ��������Ч����ФТ��Х������У�������������������������"};
const unsigned char PY_mb_xie   []={"дЩЬЪбѪлжЮмзкийШаЭегЫЯв����Ҷ��������ߢ�����������ޯ������"};
const unsigned char PY_mb_xin   []={"��������онп��Ѱ�����ضݷܰ�����"};
const unsigned char PY_mb_xing  []={"������������������������������ʡ�������ߩ��"};
const unsigned char PY_mb_xiong []={"��������������ܺ"};
const unsigned char PY_mb_xiu   []={"�����������������������������������"};
const unsigned char PY_mb_xu    []={"�����������������������������������������������ޣ���������ڼ�������"};
const unsigned char PY_mb_xuan  []={"ѡ������������ѤѣѢȯ������������������������������"};
const unsigned char PY_mb_xue   []={"ѧѩѪѥѨ��Ѧ�������"};
const unsigned char PY_mb_xun   []={"ѰѶѬѵѭѳѮѲѸѱѴѷѫ��ѯ��������ަ޹��������������ۨ�"};
const unsigned char PY_mb_ya    []={"ѽѹ��ѺѿѼ����������Ѿ����ѻ��������������������������"};
const unsigned char PY_mb_yan   []={"����������������������������������������������������������������������������������������������������ٲ۱۳�������������Ǧ����ܾ������"};
const unsigned char PY_mb_yang  []={"������������������������������������������������"};
const unsigned char PY_mb_yao   []={"Ҫҡҩҧ��ҤҨ����ҥңҦ��ҫҢԿ��زسߺ���������������������ű��Լ�"};
const unsigned char PY_mb_ye    []={"ҲҹҵҰҶүҳҺҴҸұҭҮ��ҷҬа��������������"};
const unsigned char PY_mb_yi    []={"һ��������������ҽ������������������������ʳ��������������Ҽ����Ҿҿ�����������ض���β���������������������������������������������������������ܲ����ڱ�����������������������٫���������������߮����������޲߽����"};
const unsigned char PY_mb_yin   []={"����ӡ����������������������������ܧ۴��ط���������������������"};
const unsigned char PY_mb_ying  []={"ӦӲӰӪӭӳӬӮӥӢӱӨӯӤӣӧӫө������������ݺ�۫�������������������"};
const unsigned char PY_mb_yo    []={"Ӵ���"};
const unsigned char PY_mb_yong  []={"��ӿ��ӵӼ��ӺӽӾӶӻӸӹӷ��ٸ��ܭ�����������"};
const unsigned char PY_mb_you   []={"��������������������������������������������٧ݬݯݵ������������������������������"};
const unsigned char PY_mb_yu    []={"������������������������������������������������������������������������������ξ����ԡԢԣԤԥԦε����������������������������������������خ��ٶ��������������������������ع������������"};
const unsigned char PY_mb_yuan  []={"ԶԱԪԺԲԭԸ԰ԮԳԹԩԴԵԬԨԷԫԧԯ��������������ܾ������ܫ��"};
const unsigned char PY_mb_yue   []={"��ԽԼԾ��������Ի˵��Կ����������"};
const unsigned char PY_mb_yun   []={"������������������������Ա�����۩ܿ����������"};
const unsigned char PY_mb_za    []={"����զ����������"};
const unsigned char PY_mb_zai   []={"��������������������"};
const unsigned char PY_mb_zan   []={"���������������������"};
const unsigned char PY_mb_zang  []={"�����߲������"};
const unsigned char PY_mb_zao   []={"������������������������������"};
const unsigned char PY_mb_ze    []={"��������զ�������������������"};
const unsigned char PY_mb_zei   []={"��"};
const unsigned char PY_mb_zen   []={"����"};
const unsigned char PY_mb_zeng  []={"����������������"};
const unsigned char PY_mb_zha   []={"��ը��բգեէ��թ����ա��߸�����������������դզ����"};
const unsigned char PY_mb_zhai  []={"ժխծիկ���լ������"};
const unsigned char PY_mb_zhan  []={"վռսյմճձչջղ��պտ��նշոհ�����"};
const unsigned char PY_mb_zhang []={"���³��������������������������������۵�����"};
const unsigned char PY_mb_zhao  []={"����������צ�׳������س�����������گ��"};
const unsigned char PY_mb_zhe   []={"����������������������������ߡ����������"};
const unsigned char PY_mb_zhen  []={"����������������������������������������������������������"};
const unsigned char PY_mb_zheng []={"��������������֤֢֣��������֡������ں�綡��"};
const unsigned char PY_mb_zhi   []={"ֱֻ֪֮��ָֽ֧֥֦��֨֩��֭֫֬��ְ֯��ֲ��ֳִֵֶַ����ֹֺ��ּ��־ֿ����������ʶ����������������������������ۤ���������������������������������ش�������μ�"};
const unsigned char PY_mb_zhong []={"�����������������������������ڣ��"};
const unsigned char PY_mb_zhou  []={"����������������������������������ݧ����������"};
const unsigned char PY_mb_zhu   []={"ס��������������������������������������������עףפ�����������������������٪�����������ۥ��������"};
const unsigned char PY_mb_zhua  []={"ץצ��"};
const unsigned char PY_mb_zhuai []={"קת"};
const unsigned char PY_mb_zhuan []={"תרש׬��׫׭�������"};
const unsigned char PY_mb_zhuang[]={"װײׯ׳׮״��ױ�����"};
const unsigned char PY_mb_zhui  []={"׷׹׺׶׸׵�������"};
const unsigned char PY_mb_zhun  []={"׼׻�����"};
const unsigned char PY_mb_zhuo  []={"׽������׾����׿��������ߪ��������پ�����"};
const unsigned char PY_mb_zi    []={"��������������֨��������������������������������������������������������"};
const unsigned char PY_mb_zong  []={"����������������������"};
const unsigned char PY_mb_zou   []={"����������۸������"};
const unsigned char PY_mb_zu    []={"����������������������"};
const unsigned char PY_mb_zuan  []={"����׬߬����"};
const unsigned char PY_mb_zui   []={"��������Ѿ���ީ"};
const unsigned char PY_mb_zun   []={"����ߤ����"};
const unsigned char PY_mb_zuo   []={"�������������������������������������"};

/*
*********************************************************************************************************
*	                         ƴ�������Ľṹ������
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
	{"ei",PY_mb_bei},
	{"en",PY_mb_ben},
	{"eng",PY_mb_beng},
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
	{"hong",PY_mb_chong},
	{"hou",PY_mb_chou},
	{"hu",PY_mb_chu},
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
	{"a   ",PY_mb_da},
	{"ai",PY_mb_dai},
	{"an",PY_mb_dan},
	{"ang",PY_mb_dang},
	{"ao",PY_mb_dao},
	{"e",PY_mb_de},
	{"eng",PY_mb_deng},
	{"i",PY_mb_di},
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
	{"ian",PY_mb_lian},
	{"iang",PY_mb_liang},
	{"iao",PY_mb_liao},
	{"ie",PY_mb_lie},
	{"in",PY_mb_lin},
	{"ing",PY_mb_ling},
	{"iu",PY_mb_liu},
	{"ong",PY_mb_long},
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
	{"ian",PY_mb_xiao},
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
*	                         ƴ�������Ľṹ�������ָ��
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
*	�� �� ��: PYSearch
*	����˵��: ����ƴ���ĺ���,ʹ�úܼ򵥣�ֻ������ƴ�����з������ƴ����Ӧ�ĺ��ָ�����
*	��    �Σ�msg   ƴ���ַ���
*             num   ���ش�ƴ����Ӧ�ĺ��ָ���
*	�� �� ֵ: ������ƴ�����ں��������Ӧ�����ؼ������ĺ��ֵ�ָ�룬���û�У����ؿ�ָ��
*********************************************************************************************************
*/										
char *PYSearch(unsigned char *msg , int *num)
{
	unsigned char count=0;
	unsigned char i=0;
	/*
		���确b'���Զ���Ϊ��b'��ͷ�����к���
		b'����һ����c'���Զ���Ϊ��c'��ͷ�����к���
		ͨ����Ƚ��������ݵĴ�С���Եõ���b'�����е�Ԫ��+����
	*/
	struct PinYinPCB const *headpcb; 		
	struct PinYinPCB const *tailpcb;		
											
	while(msg[count] != 0) count++;			  /* ���������ƴ�����ֽ���     */	
	
	for(i = 0; i < count; i++)				  /* ��������ƴ���Ƿ���Ϲ淶 */
	{
		if((msg[i] >= 'a') && (msg[i]<='z'))  /* ����������������Ч       */
		{
			continue;
		}								   	  /* ��������ݷǷ������ؿ�ָ�� */
		else
		{
		    return '\0';
		}
	}
	
	/* ��������ݷǷ������ؿ�ָ�� */
	if((*msg == 'i')||(*msg == 'u')||(*msg == 'v')||(*msg == '\0'))	
	{
	    return '\0';
	}
	
	/* �Զ���Ϊ*msg��ͷ������ƴ������ */
	tailpcb = PYRankThreeList[*msg - 'a'];	
	
 	/* �Զ���Ϊ*msg+1��ͷ������ƴ������ */
	headpcb = PYRankThreeList[*msg - 'a' + 1];
	
	for(; tailpcb < headpcb; tailpcb++)
	{
		
		/* ��������ƴ��������ṹ���е�ƴ���Ƿ�һ�� */
		for(i = 0; i < count - 1; i++)				
		{	
			if(*((*tailpcb).PYSerList+i) != msg[i+1])
			{
				break;
			}
		}
		
		/* һ�£�����ָ�� */
		if(i == (count - 1))	 					
		{
			/* �ر�ע�����������������ΪUTF-8�����ʽ�ĺ�����3���ֽ� */
			*num = strlen((const char *)(*tailpcb).PYRankOneList) / 3;
			return (char *)(*tailpcb).PYRankOneList;
		}

	}
	
	/* ���߷��ؿյ�ָ�� */
	return '\0';								
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
