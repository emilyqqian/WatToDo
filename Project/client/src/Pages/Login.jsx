import { useState } from 'react'
import { loginUser,getStringHashCode, getInvitation, getTaskboards, setAuthToken, tryLogin } from '../APIManager'
import { useNavigate } from 'react-router-dom';
import {
  Container,
  Typography,
  TextField,
  Button,
  Link,
  Box,
  Grid
} from '@mui/material'
import { useGlobal } from '../SessionManager';

function Login(){
    const [username, setUsername] = useState('')
    const [password, setPassword] = useState('')

    const { state, updateState } = useGlobal();

    const navigator = useNavigate();

    if (state.tryAutoLogin){
        tryLogin().then(data => {
            if (data == null){
                updateState({tryAutoLogin: false});
                return;
            }

            console.log("returned token: " + data.auth);
            setAuthToken(data.auth);

            data.password = getStringHashCode(password)
            updateState({ user: data, loggedIn: true, authToken: data.auth})
            getTaskboards(data.userId).then(addTaskboards);
            getInvitation(data.userId).then(data => updateState({ invitation: data }));
            navigator('/')
        })

        return (<h1>Loading...</h1>);
    }

    function handleLogin(){
        console.log("password hash: " + getStringHashCode(password))

        loginUser(username, password).then(data=>{
            if (data != null){
                console.log("returned token: " + data.auth);
                setAuthToken(data.auth);

                data.password = getStringHashCode(password)
                updateState({ user: data, loggedIn: true, authToken: data.auth})
                getTaskboards(data.userId).then(addTaskboards);
                getInvitation(data.userId).then(data => updateState({ invitation: data }));
                navigator('/')
            }
        });
    }

    function sort(a, b){

        // if a task is already finished put it in the back
        if ("finished_date" in a && "finished_date" in b){
          return a.finished_date < b.finished_date;
        }else if ("finished_date" in a){
          return 1
        } else if("finished_date" in b){
          return -1
        }

        if (a.pinned === b.pinned){
            if (a.due_date < b.due_date) return -1;
            else if (a.due_date > b.due_date) return 1;
            if (a.start_date < b.start_date) return -1;
            else if (a.start_date > b.start_date)return 1;

            return 0;
        }
        return a.pinned ? -1 : 1; 
    }

    function addTaskboards(data){
        data = data != null && "taskboards" in data ? data.taskboards : [];
        let privateBoards = []
        let sharedBoards = []
        console.dir(data, {depth:null})

        for (let i = 0; i < data.length; i++){            
            if (data[i].tasks === undefined) data[i].tasks = []
            data[i].tasks.sort(sort)

            if (data[i].users.length === 1){
                data[i].isShared = false;
                privateBoards.push(data[i])
            } else{
                data[i].isShared = true;
                sharedBoards.push(data[i])
            }
        }

        updateState({privateTaskboardList: privateBoards, sharedTaskboardList: sharedBoards})
    }

    return (
        <Box sx={{
        minHeight: '92vh',
        background: `linear-gradient(135deg, rgba(102, 126, 234, 0.1), rgba(118, 75, 162, 0.1)), url('/background.jpg')`,
        backgroundSize: 'cover',
        backgroundPosition: 'center',
        backgroundAttachment: 'fixed',
        py: 0,
        px: 2,
      }}>
        <Container maxWidth="xs" sx={{ py: 6 }}>
        
            <Typography variant="h3" fontWeight={600} sx={{ mb: '10%' }}>
                Login: 
            </Typography>

            <TextField
                label="Username"
                value={username}
                onChange={(event) => setUsername(event.target.value)}
                fullWidth
                sx={{mb: '5%'}}
            />

            <TextField
                label="Password"
                type="password"
                autoComplete="current-password"
                value={password}
                onChange={(event) => setPassword(event.target.value)}
                fullWidth
                sx={{mb: '5%'}}
            />
            <Grid container justifyContent="space-between">
                <Grid>
                    <Link href="/register">
                        Register New Account
                    </Link>
                </Grid>
                <Grid>
                    <Button variant="contained" onClick={handleLogin}>
                        Login
                    </Button>
                </Grid>
            </Grid>
        </Container>
        </Box>
    )
}

export default Login