#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <vector>
#include <string>
#include "gitint.h"

using namespace std;

string m(const string& a,int b);
#define DEBUG
#ifdef DEBUG
#define DEBUG_MSG(str) do { std::cout <<m(" ",15)<<"|||"<< str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif

//helper string multiplier
string m(const string& a,int b){
    string c="";
    for(int i=0;i<b;i++){
        c+=a;
    }
    return c;
}

/*********************** Messages to use for errors ***************************/
const std::string INVALID_COMMAND = "Invalid command";
const std::string INVALID_OPTION = "Invalid option";
const std::string INVALID_COMMIT_NUMBER = "Invalid commit number";
const std::string LOG_COMMIT_STARTER = "Commit: ";



// Class implementation

void GitInt::print_menu() const
{
    cout << "Menu:                          " << endl;
    cout << "===============================" << endl;
    cout << "create   filename int-value    " << endl;
    cout << "edit     filename int-value    " << endl;
    cout << "display  (filename)            " << endl;
    cout << "display  commit-num            " << endl;
    cout << "add      file1 (file2 ...)     " << endl;
    cout << "commit   \"log-message\"       " << endl;
    cout << "tag      (-a tag-name)         " << endl;
    cout << "log                            " << endl;
    cout << "checkout commit-num/tag-name   " << endl;
    cout << "diff                           " << endl;
    cout << "diff     commit                " << endl;
    cout << "diff     commit-n commit-m     " << endl;
}


bool GitInt::process_command(std::string cmd_line)
{
    bool quit = false;
    std::stringstream ss(cmd_line);
    std::string cmd;
    ss >> cmd;
    if (ss.fail()) throw std::runtime_error(INVALID_COMMAND);
    // Continue checking/processing commands
    string s_arg1,s_arg2;
    int i_arg1,i_arg2;
    
    if(cmd=="create"){
        ss>>s_arg1;
        ss>>i_arg1;
        if(ss.fail()){
            throw std::runtime_error(INVALID_COMMAND); //ERROR
        }else{
        	if((files.find(s_arg1)==files.end())){
        		create(s_arg1,i_arg1);	
        	}else{
        		throw std::invalid_argument(INVALID_OPTION);
        	}
        }
    }
    else if(cmd=="edit"){
        ss>>s_arg1;
        ss>>i_arg1;
        if(ss.fail()){
            throw std::runtime_error(INVALID_COMMAND); //ERROR
        }else{
        	if(files.find(s_arg1)!=files.end()){
        		edit(s_arg1,i_arg1);
        	}else{
        		throw std::invalid_argument(INVALID_OPTION);
        	}
        }       
    }
    else if(cmd=="display"){
    	ss>>i_arg1;
        if(ss.fail()){ // IS INT?
        	ss.clear();
        	ss=stringstream(cmd_line);
        	ss>>cmd;
        	ss>>s_arg1;
        	if(ss.fail()){
        		display_all();	
        	}else{
        		if(files.find(s_arg1)!=files.end()){
        			display(s_arg1);	
        		}else{
        			throw std::invalid_argument(INVALID_OPTION);
        		}
        	}
        }else{
        	if(valid_commit(i_arg1)){
        		display_commit(i_arg1);	
        	}else{
        		throw std::invalid_argument(INVALID_COMMIT_NUMBER);
        	}
        } 
    }
    else if(cmd=="add"){
    	while(ss>>s_arg1){
    		if(files.find(s_arg1)!=files.end()){
    			add(s_arg1);
    		}else{
    			throw std::invalid_argument(INVALID_OPTION);
    		}
    	}
    }
    else if(cmd=="commit"){
    	string txt;
    	getline(ss,txt,'"');
    	if(ss.fail() || txt!=" "){
    		throw std::runtime_error(INVALID_COMMAND);
    	}
    	getline(ss,txt,'"');
    	if(!ss.fail() && !txt.empty()){
    		commit(txt);
    	}else{
    		throw std::runtime_error(INVALID_COMMAND);
    	}
    }
    else if(cmd=="tag"){
    	ss>>s_arg1;
    	if(ss.fail()){
    		tags();
    	}else{
    		if(s_arg1!="-a"){
    			throw std::runtime_error(INVALID_COMMAND);
    		}else{
    			ss>>s_arg2;
    			if(ss.fail()){
    				throw std::runtime_error(INVALID_COMMAND);
    			}else{
    				if(tag_map.find(s_arg2)!=tag_map.end()){
    					create_tag(s_arg2,current_commit);
    				}else{
    					throw std::invalid_argument(INVALID_OPTION);
    				}
    			}
    		}
    	}
    }
    else if(cmd=="checkout"){
    	ss>>i_arg1;
    	if(ss.fail()){
    		ss.clear();
    		ss<<cmd_line;
    		ss>>cmd;
    		ss>>s_arg1;
    		if(ss.fail()){
    			throw std::runtime_error(INVALID_COMMAND);
    		}else{
    			map<string,int>::iterator i=tag_map.find(s_arg1);
    			if(i!=tag_map.end()){
    				checkout(s_arg1);
    			}else{
    				throw std::invalid_argument(INVALID_OPTION);
    			}
    		}
    	}else{
    		if (i_arg1<=(int)commits.size()){
    			checkout(i_arg1);
    		}else{
    			throw std::invalid_argument(INVALID_OPTION);
    		}
    	}
    }
    else if(cmd=="log"){
    	log();
    }
    else if(cmd=="diff"){
    	ss>>i_arg1;
    	if(ss.fail()){
    		ss>>i_arg2;
    		if(ss.fail()){
    			diff(i_arg1);
    		}else{
    			if(i_arg1>=i_arg2){
    				throw std::invalid_argument(INVALID_OPTION);
    			}else{
    				diff(i_arg1,i_arg2);
    			}
    			
    		}
    	}else{
    		diff(current_commit);
    	}
    }
    else if(cmd=="quit"){
    	quit=true;
    }
    else{
    	throw std::runtime_error(INVALID_COMMAND);
    }
    ss.clear();

    return quit;
}

/**
 * Default constructor
 * [TO BE WRITTEN]
 */
GitInt::GitInt(){
	current_commit=0;
	total_commit=0;
	staged=vector<string>();
	files=map<string,int>();
    commits=vector<CommitObj>();
    commits.push_back(CommitObj("rootCommit",map<string,int>(),-1));
    tag_map=map<string,int>();
    tag_vec=vector<string>();
}

/**
 * Creates a new file with the given value
 * [TO BE WRITTEN]
 *
 * @param[in] filename
 *    Name of the file to be created
 * @param[in] value
 *    Integer content of the file
 * @throws std::invalid_argument or std::runtime_error -
 *    See homework writeup for details on error cases
 */
void GitInt::create(const std::string& filename, int value){
    if((files.find(filename)==files.end()) && filename!=""){
        files[filename]=value;  
    }else{
        throw std::invalid_argument(INVALID_OPTION);
    }
}

/**
 * Modifies the given file to a new value
 * [TO BE WRITTEN]
 *
 * @param[in] filename
 *    Name of the file to be edited
 * @param[in] value
 *    New integer content of the file
 * @throws std::invalid_argument or std::runtime_error -
 *    See homework writeup for details on error cases
 */
void GitInt::edit(const std::string& filename, int value){
    if((files.find(filename)!=files.end()) && filename!=""){
        files[filename]=value;
    }else{
        throw std::invalid_argument(INVALID_OPTION);
    }    
}

/**
 * Displays the current contents of the given file
 * [TO BE WRITTEN]
 *
 * @param[in] filename
 *    Name of the file to be edited
 * @throws std::invalid_argument -
 *    See homework writeup for details on error cases
 */
void GitInt::display(const std::string& filename) const{
    if((files.find(filename)!=files.end()) && filename!=""){
        cout<<filename<<" : "<<files.at(filename)<<endl;
    }else{
        throw std::invalid_argument(INVALID_OPTION);
    }
}

/**
 * Displays the current content of all files
 * [TO BE WRITTEN]
 *
 */
void GitInt::display_all() const{
	display_helper(files);
}


/**
 * Stages the given file for commit
 * [TO BE WRITTEN]
 *
 * @param[in] filename
 *    File to be staged and committed on the next commit command
 * @throws std::invalid_argument or std::runtime_error -
 *    See homework writeup for details on error cases
 */
void GitInt::add(std::string filename){
    if((files.find(filename)!=files.end()) && filename!=""){
        staged.push_back(filename);
    }else{
        throw std::invalid_argument(INVALID_OPTION);
    } 
}


/**
 * Commits all files staged since the last commit
 * [TO BE WRITTEN]
 *
 * @param[in] message
 *    Log message for this commit
 * @throws std::runtime_error -
 *    See homework writeup for details on error cases
 */
void GitInt::commit(std::string message){
    if(staged.empty() || message==""){
        throw std::runtime_error(INVALID_COMMAND);
    }
	map<string,int> tmp=buildState(current_commit,0);
	map<string,int> yeet;
	for(vector<string>::iterator i=staged.begin();i!=staged.end();i++){
		if(current_commit==0){
			yeet[*i]=files[*i];
		}else{
			yeet[*i]=files[*i]-tmp[*i];
		}
	}
    if(current_commit>total_commit){
        commits.push_back(CommitObj(message,yeet,total_commit));    
    }else{
        commits.push_back(CommitObj(message,yeet,current_commit));    
    }
	total_commit++;
	current_commit=total_commit;
    staged.clear();
}

/**
 * Builds the file state between commit 'from' until 'to' (exclusive)
 * [OPTIONAL - TO BE WRITTEN]
 *
 * @param[in] from
 *    Index of the commit to start the accrual of diffs
 * @param[in] to
 *    Index of the commit to end the accrual of diffs
 * @throws may vary depending on implementation
 */
map<string, int> GitInt::buildState(CommitIdx from, CommitIdx to) const{
	map<string,int> curr;
	map<string,int> diff;

    while(from!=to){
        if(from<0){
            break;
        }
        curr=commits[from].diffs_;
        for(map<string,int>::const_iterator i=curr.begin();i!=curr.end();i++){
            diff[i->first]+=i->second;
        }
        from=commits[from].parent_;
    }

	
	return diff;
}

/** 
 * Display the file content differences between the current state back
 * through all parent/ancestor commits until the `to` commit
 * [TO BE WRITTEN]
 *
 * @param[in] to
 *    Index of the commit to end the diff process
 * @throws std::invalid_argument -
 *    See homework writeup for details on error cases
 */
void GitInt::diff(CommitIdx to) const{
    if(valid_commit(to) && to>=0){
        map<string,int> tmp=buildState(to,0);
        map<string,int> a=files;
        map<string,int> yeet;
        for(map<string,int>::const_iterator i=a.begin();i!=a.end();i++){
            if(current_commit==0){
                yeet[i->first]=a[i->first];
            }else{
                if(i->second-tmp[i->first]==0){

                }else{
                    yeet[i->first]=i->second-tmp[i->first];
                }
            }
        }
        display_helper(yeet); 
    }else{
        throw std::invalid_argument(INVALID_OPTION);
    }
}


/**
 * Display the file content differences between commit `from` back
 * through all parent/ancestor commits until the `to` commit
 * [TO BE WRITTEN]
 *
 * @param[in] from
 *    Index of the commit to start the diff process
 * @param[in] to
 *    Index of the commit to end the diff process
 * @throws std::invalid_argument -
 *    See homework writeup for details on error cases
 */
void GitInt::diff(CommitIdx from, CommitIdx to) const{
    if(valid_commit(to) && from>=0 && to>=0 && from>=to){
        map<string,int> tmp=buildState(to,0);
        map<string,int> a=buildState(from,0);
        map<string,int> yeet;
        for(map<string,int>::const_iterator i=a.begin();i!=a.end();i++){
            if(current_commit==0){
                yeet[i->first]=a[i->first];
            }else{
                if(i->second-tmp[i->first]==0){

                }else{
                    yeet[i->first]=i->second-tmp[i->first];
                }
            }
        }
        display_helper(yeet); 
    }else{
        throw std::invalid_argument(INVALID_OPTION);
    }
}

/**
 * Associates a new tag name to the currently checked-out commit
 * [TO BE WRITTEN]
 *
 * @param[in] tagname
 *    Name for this tag
 * @param[in] commit
 *    Commit number/index to associate with the given tagname
 * @throws std::invalid_argument or std::runtime_error -
 *    See homework writeup for details on error cases
 */
void GitInt::create_tag(const std::string& tagname, CommitIdx commit){
	tag_map[tagname]=commit;
	tag_vec.push_back(tagname);
}

/**
 * Displays all tag names in the order they were created from most
 * to least recent.
 * [TO BE WRITTEN]
 *
 */
void GitInt::tags() const{
	for(int i=((int)tag_vec.size())-1;i>=0;i--){
		cout<<tag_vec[i]<<endl;
	}
}

/**
 * Updates the files and content to match the state of the given commit
 * [TO BE WRITTEN]
 *
 * @param[in] commit
 *    Index of the commit to checkout
 * @throws std::invalid_argument or std::runtime_error -
 *    See homework writeup for details on error cases
 */
bool GitInt::checkout(CommitIdx commitIndex){
	if(valid_commit(commitIndex)){
		files=buildState(commitIndex);
		current_commit=commitIndex;
		return true;
	}else{
        throw std::invalid_argument(INVALID_OPTION);
		return false;
	}
}

/**
 * Updates the files and content to match the state of the given commit
 * [TO BE WRITTEN]
 *
 * @param[in] tag
 *    Tag name associated with the commit to checkout
 * @throws std::invalid_argument or std::runtime_error -
 *    See homework writeup for details on error cases
 */
bool GitInt::checkout(std::string tag){
    if(tag_map.find(tag)!=tag_map.end() && tag!=""){
        int a=tag_map[tag];
        return checkout(a);
    }else{
        throw std::invalid_argument(INVALID_OPTION);
        return false;  
    }

}

/**
 * Displays the commit numbers and log message in order from the current
 * checked-out commit back through all parent/ancestor commits.
 * [TO BE WRITTEN]
 *
 * @throws std::invalid_argument or std::runtime_error -
 *    See homework writeup for details on error cases
 */
void GitInt::log() const{
	int commit=current_commit;
    if(current_commit>total_commit) commit--;
    vector<int> comm;
	while(commit!=0){
		comm.push_back(commit);
		commit=commits[commit].parent_;
	}
    for(int i=0;i<(int)comm.size();i++){
        log_helper(comm[i],commits[comm[i]].msg_);
    }
}


/**
 *  Returns true if the given input is a valid commit number
 *  [TO BE WRITTEN]
 * @param[in] commit
 */
bool GitInt::valid_commit(CommitIdx commit) const{
	if(total_commit>=commit && commit>=0){
		return true;
	}else{
		return false;
	}
}

void GitInt::display_commit(CommitIdx commit) const
{
    if ( 0==commit || !valid_commit(commit) ) {
        throw std::invalid_argument(INVALID_COMMIT_NUMBER);
    }
    display_helper(commits.at(commit).diffs_);
}


void GitInt::display_helper(const std::map<std::string, int>& dat) const
{
    for (std::map<std::string, int>::const_iterator cit = dat.begin();
            cit != dat.end();
            ++cit) {
        std::cout << cit->first << " : " << cit->second << std::endl;
    }
}


void GitInt::log_helper(CommitIdx commit_num, const std::string& log_message) const
{
    std::cout << LOG_COMMIT_STARTER << commit_num << std::endl;
    std::cout << log_message << std::endl << std::endl;
}

