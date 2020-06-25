#include<iostream>
#include<stdio.h>
#include<memory.h>
#include<cstring>
#include<fstream>
#include<iomanip>
#include<cstdlib>
#include<ctype.h>
#include<string>
#include<map>
#include<sstream>
#include <bitset>
using namespace std;

int getReg(char* arg,map<string,int>reg_map);
int getOp(string arg,map<string,int>reg_map);
string binToHex(string bin);
string getBinStr(int num,int len);
int getImm(string arg,map<string,int>label_map);

int main(int argc, char** argv){
    string asmfile;
    if(argc==1){
        cout<<"error: please input .asm filename!"<<endl;
        exit(-1);
    }
    asmfile=argv[1];
    ifstream code(asmfile.c_str());
    if(!code.is_open()){
        cout<<"error: asm file doesn't exist!"<<endl;
        exit(-1);
    }
    ofstream out_f1(string("_temp1.asm").c_str());
    string temp;
    char s[256];

    while(code.getline(s,256)){
        //cout<<s<<endl;
        int i=0;
        while(s[i]!='#' && i<256){
            i++;
        }
        char temp_s[256]={0};
        strncpy(temp_s,s,i);
        out_f1<<temp_s<<endl;
    }

    out_f1.close();
    code.close();
    map<string,int> mymap;
    int cur_datap=0;
    int cur_textp=0x00400000;
    int data_or_text=0;//0是data,1是text
    int i=0;
    ifstream in_f1(string("_temp1.asm").c_str());
    while(in_f1.getline(s,256)){
        i=0;
        //cout<<s<<endl;
        while(isspace(s[i]))i++;

        if(i>=256 || i<0)continue;
        if(s[i]=='.'){
            if(s[i+1]=='t') data_or_text=1;
            else data_or_text=0;
            continue;
        }
        else{
            string::size_type idx1;
            string::size_type idx2;
            string temp_s=s;
            idx1=temp_s.find(":");
            if(idx1 == string::npos){
                if(!(isspace(s[i])||s[i]=='\0')){
                        istringstream is(temp_s);
                        string op_str;
                        is>>op_str;
                        if(op_str=="lw"){
                            string _str;
                            is>>_str;
                            string::size_type temp_idx=_str.find(",");
                            _str=_str.substr(temp_idx+1,10);
                            //cout<<_str<<endl;
                            map<string, int>::iterator temp_it=mymap.find(_str);
                            if(temp_it!=mymap.end() && temp_it->second<0x00400000){
                                cur_textp+=4;
                            }
                        }
                        cur_textp+=4;
                }
            }
            else{
                idx2=idx1;
                idx2--;
                while(s[idx2]!=' '&& s[idx2]!='\t'&& idx2>0) idx2--;

                if(s[idx2]==' '|| s[idx2]=='\t')idx2+=1;
                int _n=idx1-idx2;
                string name_of_label=temp_s.substr(idx2,_n);
                int _loc=0;
                if(data_or_text==0){
                    _loc=cur_datap;
                    cur_datap+=4;
                }
                else{
                    _loc=cur_textp;
                }
                pair<string,int> new_label(string(name_of_label),_loc);
                //cout<<name_of_label<<","<<_loc<<endl;
                mymap.insert(new_label);
            }
        }
    }

    for (map<string, int>::iterator it=mymap.begin(); it!=mymap.end(); it++)
    {
        //cout<<it->first<<' '<<hex<<it->second<<endl;
    }
    in_f1.clear();
    in_f1.seekg(0,ios::beg);

    ofstream out_f2(string("_temp2.asm").c_str());
    while(in_f1.getline(s,256)){

        string temp_s=s;
        string::size_type idx1=temp_s.find(":");
        string::size_type idx2=temp_s.find(".");
        if(idx1 == string::npos && idx2 == string::npos){
            int i=0;
            int j=temp_s.length();
            while(isspace(s[i]))i++;
            while(s[j]=='\0'||isspace(s[j]))j--;
            if(i>=256 || i<0)continue;
            if(s[i]=='\0')continue;
            istringstream is(temp_s);
            string op_str;
            is>>op_str;
            //cout<<op_str<<endl;
            if(op_str=="lw"){
                //cout<<"haha"<<endl;
                string _str;
                is>>_str;


                string::size_type temp_idx=_str.find(",");
                string label_str=_str.substr(temp_idx+1,10);

                map<string, int>::iterator temp_it=mymap.find(label_str);
                if(temp_it!=mymap.end() && temp_it->second<0x00400000){
                    out_f2<<"lui $at,0x1001"<<endl;
                    string::size_type start_idx=temp_s.find(s[i]);
                    string::size_type end_idx=temp_s.find(",");
                    out_f2<<temp_s.substr(start_idx,end_idx-start_idx)<<",";
                    char temp_hex[16]={0};
                    sprintf(temp_hex,"%X",temp_it->second);
                    string temp_hex1=temp_hex;
                    out_f2<<"0x"<<temp_hex1<<"($at)"<<endl;
                }
                else{
                    out_f2<<temp_s.substr(i,j-i+1)<<endl;
                }
            }
            else{
                //cout<<"haha"<<endl;
                out_f2<<temp_s.substr(i,j-i+1)<<endl;
            }
        }
    }
    out_f2.close();

    map<string,int> reg2num;
    {
        pair<string,int> reg1(string("$zero"),0);
        reg2num.insert(reg1);
        pair<string,int> reg2(string("$at"),1);
        reg2num.insert(reg2);
        pair<string,int> reg3(string("$v0"),2);
        reg2num.insert(reg3);
        pair<string,int> reg4(string("$v1"),3);
        reg2num.insert(reg4);
        pair<string,int> reg5(string("$a0"),4);
        reg2num.insert(reg5);
        pair<string,int> reg6(string("$a1"),5);
        reg2num.insert(reg6);
        pair<string,int> reg7(string("$a2"),6);
        reg2num.insert(reg7);
        pair<string,int> reg8(string("$a3"),7);
        reg2num.insert(reg8);
        pair<string,int> reg9(string("$t0"),8);
        reg2num.insert(reg9);
        pair<string,int> reg10(string("$t1"),9);
        reg2num.insert(reg10);
        pair<string,int> reg11(string("$t2"),10);
        reg2num.insert(reg11);
        pair<string,int> reg12(string("$t3"),11);
        reg2num.insert(reg12);
        pair<string,int> reg13(string("$t4"),12);
        reg2num.insert(reg13);
        pair<string,int> reg14(string("$t5"),13);
        reg2num.insert(reg14);
        pair<string,int> reg15(string("$t6"),14);
        reg2num.insert(reg15);
        pair<string,int> reg16(string("$t7"),15);
        reg2num.insert(reg16);
        pair<string,int> reg17(string("$s0"),16);
        reg2num.insert(reg17);
        pair<string,int> reg18(string("$s1"),17);
        reg2num.insert(reg18);
        pair<string,int> reg19(string("$s2"),18);
        reg2num.insert(reg19);
        pair<string,int> reg20(string("$s3"),19);
        reg2num.insert(reg20);
        pair<string,int> reg21(string("$s4"),20);
        reg2num.insert(reg21);
        pair<string,int> reg22(string("$s5"),21);
        reg2num.insert(reg22);
        pair<string,int> reg23(string("$s6"),22);
        reg2num.insert(reg23);
        pair<string,int> reg24(string("$s7"),23);
        reg2num.insert(reg24);
        pair<string,int> reg25(string("$t8"),24);
        reg2num.insert(reg25);
        pair<string,int> reg26(string("$t9"),25);
        reg2num.insert(reg26);
        pair<string,int> reg27(string("$k0"),26);
        reg2num.insert(reg27);
        pair<string,int> reg28(string("$k1"),27);
        reg2num.insert(reg28);
        pair<string,int> reg29(string("$gp"),28);
        reg2num.insert(reg29);
        pair<string,int> reg30(string("$sp"),29);
        reg2num.insert(reg30);
        pair<string,int> reg31(string("$fp"),30);
        reg2num.insert(reg31);
        pair<string,int> reg32(string("$ra"),31);
        reg2num.insert(reg32);
    }
    map<string,int> op2num;
    {
        pair<string,int> op1(string("add"),0b100000);
        op2num.insert(op1);
        pair<string,int> op2(string("addu"),0b100001);
        op2num.insert(op2);
        pair<string,int> op3(string("sub"),0b100010);
        op2num.insert(op3);
        pair<string,int> op4(string("and"),0b100100);
        op2num.insert(op4);
        pair<string,int> op5(string("or"),0b100101);
        op2num.insert(op5);
        pair<string,int> op6(string("nor"),0b100111);
        op2num.insert(op6);
        pair<string,int> op7(string("slt"),0b101010);
        op2num.insert(op7);
        pair<string,int> op8(string("sll"),0b000000);
        op2num.insert(op8);
        pair<string,int> op9(string("srl"),0b000010);
        op2num.insert(op9);
        pair<string,int> op10(string("sra"),0b000011);
        op2num.insert(op10);
        pair<string,int> op11(string("jr"),0b001000);
        op2num.insert(op11);
        pair<string,int> op12(string("addi"),0b001000);
        op2num.insert(op12);
        pair<string,int> op13(string("addiu"),0b001001);
        op2num.insert(op13);
        pair<string,int> op14(string("andi"),0b001100);
        op2num.insert(op14);
        pair<string,int> op15(string("ori"),0b001101);
        op2num.insert(op15);
        pair<string,int> op16(string("lui"),0b001111);
        op2num.insert(op16);
        pair<string,int> op17(string("lw"),0b100011);
        op2num.insert(op17);
        pair<string,int> op18(string("sw"),0b101011);
        op2num.insert(op18);
        pair<string,int> op19(string("beq"),0b000100);
        op2num.insert(op19);
        pair<string,int> op20(string("bne"),0b000101);
        op2num.insert(op20);
        pair<string,int> op21(string("slti"),0b001010);
        op2num.insert(op21);
        pair<string,int> op22(string("j"),0b000010);
        op2num.insert(op22);
        pair<string,int> op23(string("jal"),0b000011);
        op2num.insert(op23);

    }
    //预处理全部完毕,接下来进行真正的汇编环节！
    ifstream in_f2(string("_temp2.asm").c_str());
    ofstream out_f("output_file.hex");
    int PC=0x00400000;
    int line_no=0;
    while(in_f2.getline(s,256)){
        line_no++;
        PC+=4;

        string s_string=s;
        istringstream s_stream(s_string);
        string op_str;
        s_stream>>op_str;
        if(op_str=="add" || op_str=="addu" || op_str=="sub" || op_str=="and" || op_str=="or" || op_str=="nor" || op_str=="slt"){

            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            char rd_str[20];
            s_stream.getline(rd_str,20,',');

            int rd_num=getReg(rd_str,reg2num);
            if (rd_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            char rs_str[20];
            s_stream.getline(rs_str,20,',');
            int rs_num=getReg(rs_str,reg2num);
            if (rs_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            char rt_str[20];
            s_stream.getline(rt_str,20);
            int rt_num=getReg(rt_str,reg2num);
            if (rt_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            string bin_code="";
            bin_code+="000000";
            bin_code+=getBinStr(rs_num,5);
            bin_code+=getBinStr(rt_num,5);
            bin_code+=getBinStr(rd_num,5);
            bin_code+="00000";
            bin_code+=getBinStr(op_num,6);
            //cout<<s_string<<endl;
            //cout<<bin_code<<" "<<bin_code.size()<<endl;
            //cout<<binToHex(bin_code)<<endl;
            out_f<<binToHex(bin_code)<<endl;
        }
        else if(op_str=="sll" || op_str=="srl" || op_str=="sra"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rd_str[20];
            s_stream.getline(rd_str,20,',');
            int rd_num=getReg(rd_str,reg2num);
            if (rd_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            char rt_str[20];
            s_stream.getline(rt_str,20,',');
            int rt_num=getReg(rt_str,reg2num);
            if (rt_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            char shamt_cstr[20];
            s_stream.getline(shamt_cstr,20);
            string shamt_str=shamt_cstr;
            int shamt_num=getImm(shamt_str,mymap);
            string bin_code="";
            bin_code+="000000";
            bin_code+="00000";
            bin_code+=getBinStr(rt_num,5);
            bin_code+=getBinStr(rd_num,5);
            bin_code+=getBinStr(shamt_num,5);
            bin_code+=getBinStr(op_num,6);

            out_f<<binToHex(bin_code)<<endl;
        }
        else if(op_str=="jr"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rs_str[20];
            s_stream.getline(rs_str,20);
            int rs_num=getReg(rs_str,reg2num);
            if (rs_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            string bin_code="";
            bin_code+="000000";
            bin_code+=getBinStr(rs_num,5);
            bin_code+="00000";
            bin_code+="00000";
            bin_code+="00000";
            bin_code+=getBinStr(op_num,6);

            out_f<<binToHex(bin_code)<<endl;
        }
        else if(op_str=="addi" || op_str=="addiu" || op_str=="andi" || op_str=="ori"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rt_str[20];
            s_stream.getline(rt_str,20,',');
            int rt_num=getReg(rt_str,reg2num);
            if (rt_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rs_str[20];
            s_stream.getline(rs_str,20,',');
            int rs_num=getReg(rs_str,reg2num);
            if (rs_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char imm_cstr[20];
            s_stream.getline(imm_cstr,20);
            string imm_str=imm_cstr;
            int imm_num=getImm(imm_str,mymap);

            string bin_code="";
            bin_code+=getBinStr(op_num,6);
            bin_code+=getBinStr(rs_num,5);
            bin_code+=getBinStr(rt_num,5);
            bin_code+=getBinStr(imm_num,16);

            out_f<<binToHex(bin_code)<<endl;
        }
        else if(op_str=="lui"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rt_str[20];
            s_stream.getline(rt_str,20,',');
            int rt_num=getReg(rt_str,reg2num);
            if (rt_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char imm_cstr[20];
            s_stream.getline(imm_cstr,20);
            string imm_str=imm_cstr;
            int imm_num=getImm(imm_str,mymap);
            string bin_code="";
            bin_code+=getBinStr(op_num,6);
            bin_code+="00000";
            bin_code+=getBinStr(rt_num,5);
            bin_code+=getBinStr(imm_num,16);

            out_f<<binToHex(bin_code)<<endl;

        }
        else if(op_str=="lw" || op_str=="sw"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            char rt_str[20];
            s_stream.getline(rt_str,20,',');
            int rt_num=getReg(rt_str,reg2num);
            if (rt_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char imm_cstr[20];
            s_stream.getline(imm_cstr,20,'(');
            string imm_str=imm_cstr;
            int imm_num=getImm(imm_str,mymap);

            char rs_str[20];
            s_stream.getline(rs_str,20,')');
            int rs_num=getReg(rs_str,reg2num);
            if (rs_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            string bin_code="";
            bin_code+=getBinStr(op_num,6);
            bin_code+=getBinStr(rs_num,5);
            bin_code+=getBinStr(rt_num,5);
            bin_code+=getBinStr(imm_num,16);

            out_f<<binToHex(bin_code)<<endl;
        }
        else if(op_str=="beq" || op_str=="bne"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rs_str[20];
            s_stream.getline(rs_str,20,',');
            int rs_num=getReg(rs_str,reg2num);
            if (rs_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rt_str[20];
            s_stream.getline(rt_str,20,',');
            int rt_num=getReg(rt_str,reg2num);
            if (rt_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }

            char imm_cstr[20];
            s_stream.getline(imm_cstr,20);
            string imm_str=imm_cstr;
            int is_label=0;
            map<string, int>::iterator _it=mymap.find(imm_str);
            if(_it!=mymap.end()){
                is_label=1;
            }

            int imm_num=getImm(imm_str,mymap);
            imm_num=(imm_num-PC)/4;
            string bin_code="";
            bin_code+=getBinStr(op_num,6);
            bin_code+=getBinStr(rs_num,5);
            bin_code+=getBinStr(rt_num,5);
            bin_code+=getBinStr(imm_num,16);

            out_f<<binToHex(bin_code)<<endl;
        }
        else if(op_str=="slti"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rt_str[20];
            s_stream.getline(rt_str,20,',');
            int rt_num=getReg(rt_str,reg2num);
            if (rt_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char rs_str[20];
            s_stream.getline(rs_str,20,',');
            int rs_num=getReg(rs_str,reg2num);
            if (rs_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char imm_cstr[20];
            s_stream.getline(imm_cstr,20);
            string imm_str=imm_cstr;
            int imm_num=getImm(imm_str,mymap);
            string bin_code="";
            bin_code+=getBinStr(op_num,6);
            bin_code+=getBinStr(rs_num,5);
            bin_code+=getBinStr(rt_num,5);
            bin_code+=getBinStr(imm_num,16);
            out_f<<binToHex(bin_code)<<endl;
        }
        else if(op_str=="j" || op_str=="jal"){
            int op_num=getOp(op_str,op2num);
            if (op_num == -1){
                cout << "Syntax error at Line " << line_no << "." << endl;
                exit(-1);
            }
            char imm_cstr[30];
            s_stream.getline(imm_cstr,30);
            string imm_str=imm_cstr;
            int p=0;
            while(isspace(imm_str[p]))p++;
            imm_str=imm_str.substr(p,imm_str.size()-p);
            int imm_num=getImm(imm_str,mymap);
            imm_num=imm_num/4;

            string bin_code="";
            bin_code+=getBinStr(op_num,6);
            bin_code+=getBinStr(imm_num,26);
            out_f<<binToHex(bin_code)<<endl;

        }
    }
    cout<<"compile finished!"<<endl;
}
            //cout<<s_string<<endl;
            //cout<<bin_code<<" "<<bin_code.size()<<endl;
            //cout<<binToHex(bin_code)<<endl;
int getReg(char* arg,map<string,int>reg_map){
    char* p=arg;
    while (isspace(*p))p++;       // Skip the space at the beginning
    if (*p != '$') return -1;

    // Parse
    string reg_str=p;
    map<string, int>::iterator temp_it=reg_map.find(reg_str);
    if(temp_it!=reg_map.end()){

        return temp_it->second;
    }
    else{
        return -1;
    }

}

int getOp(string arg,map<string,int>op_map){
    int p = 0;
    while (arg[p]==' ') p++;       // Skip the space at the beginning

    int str_len=arg.length()-p;
    // Parse
    string op_str=arg.substr(p,str_len);

    map<string, int>::iterator temp_it=op_map.find(op_str);
    if(temp_it!=op_map.end()){
        return temp_it->second;
    }
    else{
        return -1;
    }
}
string getBinStr(int num,int len){
    int sign=1;
    if(num<0)sign=-1;
    string sr="";
    while(num){
        if(num%2)sr="1"+sr;
        else sr="0"+sr;

        num/=2;
    }

    if (sr.length()<len){
        string zeros="";
        int need=len-sr.length();
        while(need>0){
            zeros+="0";
            need-=1;
        }
        sr=zeros+sr;

    }

    if (sign == -1){
        int p = sr.size() - 1;
        while (p >=0 && sr[p] != '1') p--;      // Find the last '1'
        if (p >= 0 && sr[p] == '1') p--;         // Skip the last '1'
        while (p >= 0) {
            sr[p] = ('0' + '1') - sr[p];
            p--;
        }
    }

    return sr;
}

string binToHex(string bin){
    if(bin.size()!=32){
        cout<<"Code error."<<endl;
        exit(-1);
    }
    string hex="";
    int bin_len=bin.size();
    int p=0;
    while(p<bin_len){
        string n_str=bin.substr(p,4);
        if(n_str=="0000") hex+="0";
        else if(n_str=="0001") hex+="1";
        else if(n_str=="0010") hex+="2";
        else if(n_str=="0011") hex+="3";
        else if(n_str=="0100") hex+="4";
        else if(n_str=="0101") hex+="5";
        else if(n_str=="0110") hex+="6";
        else if(n_str=="0111") hex+="7";
        else if(n_str=="1000") hex+="8";
        else if(n_str=="1001") hex+="9";
        else if(n_str=="1010") hex+="a";
        else if(n_str=="1011") hex+="b";
        else if(n_str=="1100") hex+="c";
        else if(n_str=="1101") hex+="d";
        else if(n_str=="1110") hex+="e";
        else if(n_str=="1111") hex+="f";
        else{
            cout<<"Code error."<<endl;
            exit(-1);
        }
        p+=4;
    }
    return hex;
}
int getImm(string arg,map<string,int>label_map){
    map<string, int>::iterator temp_it=label_map.find(arg);
    if(temp_it!=label_map.end()){
        return temp_it->second;
    }
    string::size_type idx=arg.find("x");//判断是否是16进制还是10进制,假设不存在2进制的
    if(idx == string::npos){    //DEC
        char num[20];
        strcpy(num,arg.c_str());
        int val=0;
        sscanf(num,"%d",&val);
        return val;
    }
    else{   //HEX
        char num[20];
        strcpy(num,arg.c_str());
        int val=0;
        sscanf(num,"%x",&val);
        return val;
    }

}
