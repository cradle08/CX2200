//alg2201��Ŀ
static void test_alg2201(BOOL8 flag)
{
	// �����б�
	nParaList modeParas;	// ģʽ����
	dParaList systemParas;	// ϵͳ����
	dParaList configParas;	// ���ò���
	dParaList reportParas;	// �������
	dParaList featureParas;	// �����������м���Ϣ���м�״̬��
	dParaList flagParas;	// ��������
	histInforList histList;	// ֱ��ͼ��Ϣ//WBC/RBC/PLT

	// �����ļ���Ϣ
	char plsFile[200]  = {0};
	char *fileDataBuffer = NULL;
	long fileSize = 0;
	long dataSize = 0;

	// �м�״̬��Ϣ��ʼ��
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

	//ֱ��ͼ����
	double histData[400] = {0}; //���400fL

	// �㷨�������
	alg_data_in dataIn;
	alg_data_out dataOut;

	// ������
	histInfor *wbcDispHist;
	char messageDest[30] = {0};

	// ��������������
	int lossSum = 0;

	if ( !flag )
	{
		return;
	}

	// ��ʼ��
	memset(&dataIn, 0, sizeof(alg_data_in));
	memset(&dataOut, 0, sizeof(alg_data_out));
	initParaListNP(&modeParas);
	initParaListDP(&systemParas);
	initParaListDP(&reportParas);
	initParaListDP(&configParas);
	initParaListDP(&featureParas);
	initParaListDP(&flagParas);
	initHistList(&histList);

	////////1.���ò���////////	
	usersMode = UM_EXPERT;
	setModePara(&modeParas, "AnalysisMode",		AM_WBC_HGB);
	setModePara(&modeParas, "SamplingVialMode",	SVM_OPEN);
	setModePara(&modeParas, "SpecimenMode",		SM_PVB);
	setModePara(&modeParas, "SpeciesMode",		SPM_HUMAN);
	setModePara(&modeParas, "FunctionMode",		FM_SAMPLE);
	setModePara(&modeParas, "AlgMode",			AM_DEBUG);
	setModePara(&modeParas, "UsersMode",		usersMode);

	////////2.��������////////
	//��ȡ����ģʽ
	nModePara = modePara(&modeParas, "AnalysisMode");

	// WBC
	if (am_hasWbcMode(nModePara))
	{
		memset(plsFile, 0, 200 * sizeof(char));
		strcpy( plsFile, "E:/Code/Release/XQ100/trunk/build/src/testbench/data/");
		strcat( plsFile, "20201109_115611_����-6.wbc.pls");
		do 
		{
			// ���ļ�������
			fileDataBuffer = plsRead(plsFile, fileDataBuffer, &fileSize);
			if (!fileDataBuffer)
			{
				break;
			}

			//ע��countTime��volumnTimeʵ����APP����
			smpRate = 1.4e6; // WBCͨ�����ݲ���Ƶ�ʣ���λHz
			countTime  = 1.0 * fileSize / sizeof(uint16) / smpRate;  // ��������ʱ�䣨��������ݶ�Ӧ��
			volumnTime = countTime + 5; // ��������ʱ�䣨��ʼ�Ӹ�ѹ������������������ʵ������λ�����
			setSystemPara(&systemParas, "WbcCalFactor", 1.0, 2);
			setSystemPara(&systemParas, "WbcDilutionRatio", 501.0, 1);
			setSystemPara(&systemParas, "WbcVolumn", 800, 2);//�¿2020.06.01֮�󣩼��������Ϊ1046.26����λuL
			setSystemPara(&systemParas, "WbcCountTime", countTime, 2);//��λs
			setSystemPara(&systemParas, "WbcVolumnTime", volumnTime, 2);//��λs
			setSystemPara(&systemParas, "WbcSmpRate", smpRate, 2);//��λHz

			// ������Ϣ
			wbcPulseInfo =  (pulse_info_i_t *)malloc(PRE_PULSE_NUM*sizeof(pulse_info_i_t));
			if (!wbcPulseInfo)
			{
				break;
			}
			pulseInit(wbcPulseInfo);

			pulseMidFeatureInit(&midFeature);
			midFeature.smpRate = smpRate;
			// ����ʶ��
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

			// ��������������
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
		setSystemPara(&systemParas, "HgbBlankADC", 2668, 1);//��ɫ�������Ѫ����ADCֵ

		do
		{
			// ���ļ�������
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

	// ������
	status = calculate(&dataOut, &dataIn);

	// ������
	// ֱ��ͼ
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

	// �������
	outputListDP(&reportParas, TRUE);

	// ������Ϣ���
	printf("\n\n������Ϣ:\n");
	for (i= 0; i<flagParas.num; i++)
	{
		if( flagParas.paras[i].flag )
		{
			// WBCͨ������
			if (flagParaItemMessageMapWbc(messageDest, flagParas.paras[i].name, usersMode, TRUE))
			{
				printf("%s\n", messageDest);
			}
			
			// HGBͨ������
			if (flagParaItemMessageMapHgb(messageDest, flagParas.paras[i].name, usersMode, TRUE))
			{
				printf("%s\n", messageDest);
			}			
		}
	}
	// ��ӡϵͳ����
	// outputListDP(&systemParas, TRUE);

	// �汾��Ϣ
	printf("\n�汾��Ϣ:\n");
	printf("alg_version = %s\t",version());
	printf("alg_release_Date = %s\n",versionDate());

	//�ͷ��ڴ�
	free(wbcPulseInfo);
	wbcPulseInfo = NULL;
	free(hgbDataBuffer);
	hgbDataBuffer = NULL;
}