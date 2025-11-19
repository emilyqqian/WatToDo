import { useState } from 'react'
import { loginUser,getStringHashCode, getLeaderboard, getTaskboards } from '../APIManager'
import { useNavigate } from 'react-router-dom';
import {
  Container,
  Typography,
  TextField,
  Button,
  Link,
  Grid
} from '@mui/material'
import { useGlobal } from '../SessionManager';

function Login(){
    const [username, setUsername] = useState('')
    const [password, setPassword] = useState('')

    const { updateState } = useGlobal();

    const navigator = useNavigate();

    function handleLogin(){
        loginUser(username, password).then(data=>{
            if (data != null){
                data.password = getStringHashCode(password)
                updateState({ user: data, loggedIn: true})
                getTaskboards(data.userId).then(addTaskboards);
                navigator('/')
            }
        });
    }

    function sort(a, b){
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
        data = data.taskboards;
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
                <Grid item>
                    <Link href="/register">
                        Register New Account
                    </Link>
                </Grid>
                <Grid item>
                    <Button variant="contained" onClick={handleLogin}>
                        Login
                    </Button>
                </Grid>
            </Grid>
        </Container>
    )
}

export default Login