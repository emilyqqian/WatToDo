import { useState } from 'react'
import { loginUser } from '../APIManager'
import { useNavigate } from 'react-router-dom';
import {
  Box,
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
                updateState({ user: data, loggedIn: true})
                navigator('/')
            }
        });
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