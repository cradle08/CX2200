//alg2201项目
static void test_alg2201(BOOL8 flag)
{
	// 参数列表
	nParaList modeParas;	// 模式参数
	dParaList systemParas;	// 系统参数
	dParaList configParas;	// 配置参数
	dParaList reportParas;	// 报告参数
	dParaList featureParas;	// 特征参数（中间信息、中间状态）
	dParaList flagParas;	// 报警参数
	histInforList histList;	// 直方图信息//WBC/RBC/PLT

	// 设置文件信息
	char plsFile[200]  = {0};
	char *fileDataBuffer = NULL;
	long fileSize = 0;
	long dataSize = 0;

	// 中间状态信息初始化
	pulse_info_i_t *wbcPulseInfo = NULL;
	uint32 wbcPulseNum = 0;
	char *hgbDataBuffer = NULL;

	double64 smpRate;
	pulse_mid_feature midFeature;
	int usersMode = 0;

	int i;
	char *ptr;
	BOOL8 status;
	BOOL8 isDataLoss = FALSE;
	int nModePara;
	double countTime;
	double volumnTime;

	//直方图数据
	double histData[400] = {0}; //最大400fL

	// 算法输入输出
	alg_data_in dataIn;
	alg_data_out dataOut;

	// 结果输出
	histInfor *wbcDispHist;
	char messageDest[30] = {0};

	// 数据完整性评价
	int lossSum = 0;

	if ( !flag )
	{
		return;
	}

	// 初始化
	memset(&dataIn, 0, sizeof(alg_data_in));
	memset(&dataOut, 0, sizeof(alg_data_out));
	initParaListNP(&modeParas);
	initParaListDP(&systemParas);
	initParaListDP(&reportParas);
	initParaListDP(&configParas);
	initParaListDP(&featureParas);
	initParaListDP(&flagParas);
	initHistList(&histList);

	////////1.设置参数////////	
	usersMode = UM_EXPERT;
	setModePara(&modeParas, "AnalysisMode",		AM_WBC_HGB);
	setModePara(&modeParas, "SamplingVialMode",	SVM_OPEN);
	setModePara(&modeParas, "SpecimenMode",		SM_PVB);
	setModePara(&modeParas, "SpeciesMode",		SPM_HUMAN);
	setModePara(&modeParas, "FunctionMode",		FM_SAMPLE);
	setModePara(&modeParas, "AlgMode",			AM_DEBUG);
	setModePara(&modeParas, "UsersMode",		usersMode);

	////////2.配置数据////////
	//获取分析模式
	nModePara = modePara(&modeParas, "AnalysisMode");

	// WBC
	if (am_hasWbcMode(nModePara))
	{
		memset(plsFile, 0, 200 * sizeof(char));
		strcpy( plsFile, "E:/Code/Release/XQ100/trunk/build/src/testbench/data/");
		strcat( plsFile, "20201109_115611_颗粒-6.wbc.pls");
		do 
		{
			// 从文件读数据
			fileDataBuffer = plsRead(plsFile, fileDataBuffer, &fileSize);
			if (!fileDataBuffer)
			{
				break;
			}

			//注：countTime和volumnTime实际由APP传递
			smpRate = 1.4e6; // WBC通道数据采样频率，单位Hz
			countTime  = 1.0 * fileSize / sizeof(uint16) / smpRate;  // 采样计数时间（与采样数据对应）
			volumnTime = countTime + 5; // 定量计数时间（开始加负压计数到定量结束），实际由下位机检测
			setSystemPara(&systemParas, "WbcCalFactor", 1.0, 2);
			setSystemPara(&systemParas, "WbcDilutionRatio", 501.0, 1);
			setSystemPara(&systemParas, "WbcVolumn", 800, 2);//新款（2020.06.01之后）计数池体积为1046.26，单位uL
			setSystemPara(&systemParas, "WbcCountTime", countTime, 2);//单位s
			setSystemPara(&systemParas, "WbcVolumnTime", volumnTime, 2);//单位s
			setSystemPara(&systemParas, "WbcSmpRate", smpRate, 2);//单位Hz

			// 脉冲信息
			wbcPulseInfo =  (pulse_info_i_t *)malloc(PRE_PULSE_NUM*sizeof(pulse_info_i_t));
			if (!wbcPulseInfo)
			{
				break;
			}
			pulseInit(wbcPulseInfo);

			pulseMidFeatureInit(&midFeature);
			midFeature.smpRate = smpRate;
			// 脉冲识别
			for (i = 0; i<(int)(fileSize/DATA_UINT_SIZE); i++)
			{
				ptr = fileDataBuffer + i*DATA_UINT_SIZE;
				status = wbcPulseIdentify(wbcPulseInfo, &wbcPulseNum, (const uint8 *)ptr, DATA_UINT_SIZE, &midFeature);
				dataSize++;

				if (!status)
				{
					break;
				}
			}

			// 数据完整性评价
			if (FALSE)
			{
				dataSize = dataSize*DATA_UINT_SIZE;
				isDataLoss = evaData( wbcPulseInfo, wbcPulseNum, &lossSum);
				printf("alg_wbcIterNum = %d\n",midFeature.iterNum);
				printf("wbcPulseNum = %d\n",wbcPulseNum);
				if (isDataLoss)
				{
					printf("dataLoss = %d bytes\n",lossSum);
					printf("dataLoss percentage = %0.3f%c\n",100.0*lossSum/dataSize,'%');
				}
				printf("\n");
			}

			dataIn.wbcPulseInfo = wbcPulseInfo;
			dataIn.wbcPulseNum = wbcPulseNum;

			free(fileDataBuffer);
			fileDataBuffer = NULL;

		} while (0);
	}

	// HGB
	if (am_hasHgbMode(nModePara))
	{
		memset(plsFile, 0, 200 * sizeof(char));
		strcpy( plsFile, "E:/Code/Release/XQ2/build/src/testbench/data/HGB/");
		strcat( plsFile, "20170425_081134_HGB-blank1-03.hgb.pls");

		setSystemPara(&systemParas, "HgbDilutionRatio", 501.0, 1);
		setSystemPara(&systemParas, "HgbBlankADC", 2668, 1);//比色皿加入溶血剂的ADC值

		do
		{
			// 从文件读数据
			hgbDataBuffer = plsRead(plsFile, hgbDataBuffer, &fileSize);
			if (!hgbDataBuffer)
			{
				break;
			}
			dataIn.hgbDataBuffer = hgbDataBuffer;
		}
		while (0);
	}

	dataIn.modeParas = &modeParas;
	dataIn.systemParas = &systemParas;
	dataIn.configParas = &configParas;
	dataOut.reportParas = &reportParas;
	dataOut.featureParas = &featureParas;
	dataOut.flagParas = &flagParas;
	dataOut.histList = &histList;

	// 主程序
	status = calculate(&dataOut, &dataIn);

	// 结果输出
	// 直方图
	wbcDispHist = getHistInfo(&histList, "WbcDispHistInfo");
	if (wbcDispHist && FALSE)
	{
		printf("%s:\n",wbcDispHist->name);
		for (i= 0; i<wbcDispHist->dataLen; i++)
		{
			printf("histData[%03d] = %0.2f\t",i,wbcDispHist->datas[i]);
			if ( i%3 == 0 || i == wbcDispHist->dataLen - 1 )
			{
				printf("\n");
			}
		}
	}

	// 参数结果
	outputListDP(&reportParas, TRUE);

	// 报警信息输出
	printf("\n\n报警信息:\n");
	for (i= 0; i<flagParas.num; i++)
	{
		if( flagParas.paras[i].flag )
		{
			// WBC通道报警
			if (flagParaItemMessageMapWbc(messageDest, flagParas.paras[i].name, usersMode, TRUE))
			{
				printf("%s\n", messageDest);
			}
			
			// HGB通道报警
			if (flagParaItemMessageMapHgb(messageDest, flagParas.paras[i].name, usersMode, TRUE))
			{
				printf("%s\n", messageDest);
			}			
		}
	}
	// 打印系统参数
	// outputListDP(&systemParas, TRUE);

	// 版本信息
	printf("\n版本信息:\n");
	printf("alg_version = %s\t",version());
	printf("alg_release_Date = %s\n",versionDate());

	//释放内存
	free(wbcPulseInfo);
	wbcPulseInfo = NULL;
	free(hgbDataBuffer);
	hgbDataBuffer = NULL;
}