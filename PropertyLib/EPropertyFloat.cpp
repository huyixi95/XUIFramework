/* Generated by Together */

#include "stdafx.h"
#include "IPropertyHost.h"
#include "EPropListCtrl.h"
#include "EPropertyFloat.h"
#include "EPropWndEdit.h"
#include "math.h"

EPropertyFloat* EPropertyFloat::SetScale( const double* pMultiply , const double* pDivide )
{
	m_pScaleMul = pMultiply;
	m_pScaleDiv = pDivide;
	return this;
}
EPropertyFloat* EPropertyFloat::SetUnit( const CString* pUnitSymbol )
{
	m_pUnitSymbol = pUnitSymbol;
	return this;
}
EPropertyFloat* EPropertyFloat::SetScale( double dScale )
{
	m_dScale    = dScale;
	return this;
}
EPropertyFloat* EPropertyFloat::SetStyle( UINT nFormatFlags , int nNumDecimals )
{
	m_nFormatFlags = nFormatFlags;
	m_nNumDecimals = nNumDecimals;
	return this;
}
EPropertyFloat::~EPropertyFloat()
{
}
EPropertyFloat::EPropertyFloat( IPropertyHost* pHost , const CString& sName , float* pProperty , const CString& sUnit , double dMinAllowed , double dMaxAllowed , bool bEnabled ) : EIconTextButtonProperty( pHost , sName , pProperty , bEnabled )
{
	m_dMin         = dMinAllowed;
	m_dMax         = dMaxAllowed;
	m_dScale       = 1.0;
	m_sFixedUnit   = sUnit;
	m_pUnitSymbol  = &m_sFixedUnit;
	m_pScaleMul    = NULL;
	m_pScaleDiv    = NULL;
	m_nNumDecimals = -1;
}
bool EPropertyFloat::OnPropApply()
{
	ASSERT( m_bSelected == true );
	ASSERT( m_pEdit     != NULL );
	ASSERT( IsEnabled() == true );

	bool bPropertyChanged = false;

	double d1;
	double d2;
	float dGuiValue;

	ASSERT( m_bSelected == true );

	ASSERT( m_pEdit != NULL );

	CString s;
	m_pEdit->GetWindowText(s);

	if( s != m_sInitialValueText )				//not the same
	{
		float dOldGuiValue = *(float*)m_pProperty * (float)GetScale();

		bool bValid = true;

		s.Remove(' ');

		if( sscanf(s, _T("%lf/%lf"), &d1, &d2)==2 )
		{
			dGuiValue = (float)(d1/d2);
		}
		else if( sscanf(s,_T("%lf*%lf"),&d1,&d2)==2 )
		{
			dGuiValue = (float)(d1*d2);
		}
		else if( sscanf(s, _T("%lf+%lf"),&d1,&d2)==2 )
		{
			dGuiValue = (float)(d1+d2);
		}
		else if( sscanf(s, _T("%lf-%lf"),&d1,&d2)==2 )
		{
			dGuiValue = (float)(d1-d2);
		}
		else if( sscanf(s, _T("%lf"),&d1)==1 )			//normal situation, entering a new number
		{
			dGuiValue = (float)d1;
		}
		else if( sscanf(s, _T("/%lf"),&d1)==1 )
		{
			dGuiValue = (float)(dOldGuiValue/d1);
		}
		else if( sscanf(s, _T("*%lf"),&d1)==1 )
		{
			dGuiValue =(float)(dOldGuiValue*d1);
		}
		else if( sscanf(s, _T("+%lf"),&d1)==1 )
		{
			dGuiValue = (float)(dOldGuiValue+d1);
		}
		else if( sscanf(s, _T("-%lf"),&d1)==1 )
		{
			dGuiValue = (float)(dOldGuiValue-d1);
		}
		else
		{
			bValid = false;
		}

		float dVariableValue = (float)(dGuiValue / GetScale());		//gui -> var

		if( bValid )
		{
			if( dVariableValue != *(float*)m_pProperty )	//not the same
			{
				if( (m_dMin==0 && m_dMax==0) || (m_dMin<=dVariableValue && dVariableValue<=m_dMax) )	//within range?
				{
					if( m_pHost->PropertyChanging(m_pProperty,&dVariableValue) )		//true if ok to change property
					{
						*(float*)m_pProperty = dVariableValue;
						bPropertyChanged = true;
					}
				}
			}
		}
	}

	return bPropertyChanged;
}
//CString EPropertyFloat::GetTextValue()
//{
//	float d = *(float*)m_pProperty;
//
//	CString s;
//
//	CString sFormat = "%lf";
//
//	if( m_nNumDecimals!=-1 )
//	{
//		sFormat.Format( "%%.%ilf" , m_nNumDecimals );
//	}
//
//	s.Format( sFormat , d * GetScale() );
//
//	bool bHasComma = (s.Find(',')>0) || (s.Find('.')>0);
//	
//	if( bHasComma )
//	{
//		s.TrimRight('0');
//		s.TrimRight('.');
//		s.TrimRight(',');
//	}
//
//	if( m_bFormatThousands )
//	{
//		s = FormatThousands(s);
//	}
//	
//	if( m_pUnitSymbol != NULL )
//	{
//		CString sUnit = *m_pUnitSymbol;
//
//		if( sUnit.GetLength() > 0 )
//		{
//			s += " ";
//			s += sUnit;
//		}
//	}
//
//	return s;
//}
CString EPropertyFloat::GetTextValue()
{
	float d = *(float*)m_pProperty;

	CString s;

	CString sFormat;

	if( ( m_nFormatFlags&EIconTextButtonProperty::SCIENTIFIC) == EIconTextButtonProperty::SCIENTIFIC )
	{
		if( m_nNumDecimals!=-1 )
			sFormat.Format( "%%1.%ie", m_nNumDecimals );
		else
			sFormat.Format( "%e" );

		s.Format( sFormat , d * GetScale() );
	}
	else
	{
		if( m_nNumDecimals!=-1 )
			sFormat.Format( "%%.%if" , m_nNumDecimals );
		else
			sFormat = "%f";

		s.Format( sFormat , d * GetScale() );

		bool bHasComma = (s.Find(',')>0) || (s.Find('.')>0);
		
		if( bHasComma )
		{
			s.TrimRight('0');
			s.TrimRight('.');
			s.TrimRight(',');
		}
		
		if( ( m_nFormatFlags&EIconTextButtonProperty::THOUSANDS) == EIconTextButtonProperty::THOUSANDS )
		{
			s = FormatThousands(s);
		}
	}
	
	if( m_pUnitSymbol != NULL )
	{
		CString sUnit = *m_pUnitSymbol;

		if( sUnit.GetLength() > 0 )
		{
			s += " ";
			s += sUnit;
		}
	}

	return s;
}

bool EPropertyFloat::HasEdit()
{
	return true;
}
double EPropertyFloat::GetScale()
{
	double dScale = m_dScale;

	if( m_pScaleMul != NULL )
	{
		dScale *= (*m_pScaleMul);
	}

	if( m_pScaleDiv != NULL )
	{
		dScale /= (*m_pScaleDiv);
	}

	return dScale;
}
