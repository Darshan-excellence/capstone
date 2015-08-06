
int dummy ()
{
	return 0;
}

void M68K_init(MCRegisterInfo *MRI)
{
	MCRegisterInfo_InitMCRegisterInfo(MRI, XCoreRegDesc, 17,
			0, 0,
			XCoreMCRegisterClasses, 2,
			0, 0,
			XCoreRegDiffLists,
			0,
			XCoreSubRegIdxLists, 1,
			0);
}

