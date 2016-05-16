	if (clientActive){
		if(isCalled){
			ostringstream result ;
			Object* obj = (Object*)cvGetSeqElem(object, 0);
			if(object->total <1)
			{
				result << "NotFound" <<endl;
			}
			for(int i = 0; i < object->total; i++){
//				CvPoint2D32f temp = convertToReal(obj[i].centre);
				result << obj[i].name  <<" " << obj[i].centre.x << " " << obj[i].centre.x  <<" Data : " << i <<endl;
			}	
			rslt = result.str();
			isCalled = false;
		}			
	}
	else{
		findContours(input,edgeImg);
		markObjects(input);
//		CvPoint2D32f temp = convertToReal(detectedObj.centre);
		Object* obj = (Object*)cvGetSeqElem(object, 0);
		ostringstream result ;
		if(object->total <1)
		{
			cout << "NotFound" <<endl;
		}
		for(int i = 0; i < object->total; i++){

//			cout << obj[i].name  <<" " << obj[i].centre.x << " " << obj[i].centre.x  <<" Data : " << i <<endl;
			result << obj[i].name  <<" " << obj[i].centre.x << " " << obj[i].centre.x  <<" Data : " << i <<endl;
		}
		rslt = result.str();
	}
